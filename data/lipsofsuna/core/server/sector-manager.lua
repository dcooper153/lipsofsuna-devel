--- Implements partitioning and swapping for the world map.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.sector_manager
-- @alias SectorManager

local Class = require("system/class")
local SectorLoader = require("core/server/sector-loader")
local Sectors = require("system/sectors")
local Timer = require("system/timer")

--- Manages map sectors.
-- @type SectorManager
local SectorManager = Class("SectorManager")

--- Creates a new sector manager.
-- @param clss SectorManager class.
-- @param database Database, or nil.
-- @return SectorManager.
SectorManager.new = function(clss, database)
	local self = Class.new(clss)
	self.database = database
	self.loaders = {}
	self.sectors = {}
	self.unload_time = 10
	-- Initialize the database tables needed by us.
	if self.database then
		self.database:query("CREATE TABLE IF NOT EXISTS objects (id INTEGER PRIMARY KEY,sector UNSIGNED INTEGER,data TEXT);");
		self.database:query("CREATE TABLE IF NOT EXISTS terrain (sector INTEGER PRIMARY KEY,data BLOB);");
	end
	return self
end

--- Reads a sector from the database.
-- @param self SectorManager.
-- @param sector Sector ID.
SectorManager.load_sector = function(self, sector)
	-- Only load once.
	if self.sectors[sector] then return end
	self.sectors[sector] = true
	self.sectors_iterator = nil
	-- Create a sector loader.
	if Server.initialized then
		self.loaders[sector] = SectorLoader(self, sector)
		self.loaders_iterator = nil
	end
	-- Handle client side terrain swapping.
	if Game.mode == "join" then
		Client.terrain_sync:load_sector(sector)
	end
end

--- Increases the timestamp of the sectors inside the given sphere.
-- @param point Position vector, in world units.
-- @param radius Refresh radius, in world units.
SectorManager.refresh = function(self, point, radius)
	Sectors:refresh(point, radius or 10)
end

--- Saves a sector to the database.
-- @param self SectorManager.
-- @param sector Sector ID.
SectorManager.save_sector = function(self, sector)
	if not self.database then return end
	self:wait_sector_load(sector)
	-- Write objects.
	if Server.initialized then
		Server.object_database:save_sector_objects(sector)
	end
	-- Write terrain.
	self.database:query("DELETE FROM terrain WHERE sector=?;", {sector})
	local data = Voxel:copy_region{sector = sector}
	self.database:query("INSERT INTO terrain (sector,data) VALUES (?,?);", {sector, data})
end

--- Saves all active sectors to the database.
-- @param self SectorManager.
-- @param erase True to completely erase the old map.
-- @param progress Progress callback.
SectorManager.save_world = function(self, erase, progress)
	if not self.database then return end
	self.database:query("BEGIN TRANSACTION;")
	-- Erase old world from the database.
	if erase then
		self.database:query("DELETE FROM terrain;")
	end
	-- Write the new world data.
	local sectors = Sectors:get_sectors()
	if progress then
		-- Write with progress updates.
		local total = 0
		for k in pairs(sectors) do total = total + 1 end
		local i = 1
		for k,v in pairs(sectors) do
			self:save_sector(k)
			progress(i / total)
			i = i + 1
		end
	else
		-- Write without progress updates.
		for k,v in pairs(sectors) do
			self:save_sector(k)
		end
	end
	self.database:query("END TRANSACTION;")
end

--- Unloads the world without saving.<br/>
--
-- Unrealizes all objects and destroys all sectors of the world.
-- You usually want to do this when you're about to quit a game and
-- start a new one soon after.
--
-- @param self SectorManager.
SectorManager.unload_all = function(self)
	-- Clear the dictionaries.
	self.loaders = {}
	self.loaders_iterator = nil
	self.sectors = {}
	self.sectors_iterator = nil
	-- Unrealize all objects.
	for k,v in pairs(__objects_realized) do
		k:detach()
	end
	-- Unload the engine sectors.
	Sectors:unload_all()
end

--- Unloads a sector without saving.<br/>
--
-- Unrealizes all normal objects in the sector and clears the terrain in the sector.
-- The sector is completely removed from the sector list of the engine.
--
-- @param self SectorManager.
-- @param sector Sector ID.
SectorManager.unload_sector = function(self, sector)
	-- Remove from the dictionaries.
	self.sectors[sector] = nil
	self.sectors_iterator = nil
	self.loaders[sector] = nil
	self.loaders_iterator = nil
	-- Unrealize all objects.
	for k,v in pairs(Game.objects:find_by_sector(sector)) do
		v:detach()
	end
	-- Unload the engine sector.
	Sectors:unload_sector(sector)
end

--- Unloads sectors that have been inactive long enough.
-- @param self SectorManager.
-- @param secs Seconds since the last update.
SectorManager.update = function(self, secs)
	-- Load sectors for players.
	--
	-- Sectors populared by players need extra priority because it's
	-- necessary that the terrain exists when players enter. Otherwise,
	-- they could fall to emptiness when there are a lot of sectors
	-- being loaded simulataneously.
	if Server.initialized then
		for k,v in pairs(Server.players_by_client) do
			local key = v:get_sector()
			local loader = key and self.loaders[key]
			if loader then
				loader:update(secs)
			end
		end
	end
	-- Update sector loaders.
	--
	-- This updates only one loader per frame. Iteration works like with
	-- pairs() but is restarted if new loaders are added. This allows
	-- incremental updated while accounting for the limitations of next().
	for i = 1,10 do
		local key,loader = next(self.loaders, self.loaders_iterator)
		self.loaders_iterator = key
		if loader and not loader:update(secs) then
			self.loaders[key] = nil
		end
	end
	-- Unload unused sectors.
	--
	-- Like above, this processes one sector per frame. If the sector
	-- has been unused for a while, it will be unloaded. When running a
	-- server, the sector will also be saved if it was fully loaded.
	if self.unload_time then
		local key = next(self.sectors, self.sectors_iterator)
		self.sectors_iterator = key
		local age = key and Sectors:get_sector_idle(key)
		if age and age > self.unload_time then
			if self.database and not self.loaders[key] then
				self:save_sector(key)
			end
			self:unload_sector(key)
		end
	end
end

--- Waits for a sector to finish loading.
-- @param self SectorManager.
-- @param sector Sector ID.
SectorManager.wait_sector_load = function(self, sector)
	local loader = self.loaders[sector]
	if not loader then return end
	loader:finish()
	self.loaders[sector] = nil
	self.loaders_iterator = nil
end

--- Waits for a sector to finish loading.
-- @param self SectorManager.
-- @param sector Sector ID.
SectorManager.get_sectors = function(self)
	return Sectors:get_sectors()
end

return SectorManager
