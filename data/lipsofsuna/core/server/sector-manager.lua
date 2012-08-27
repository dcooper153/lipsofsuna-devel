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
	self.update_timer = 0
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
	-- Create a sector loader.
	if Server.initialized then
		self.loaders[sector] = SectorLoader(self, sector)
	end
	-- Handle client side terrain swapping.
	if Game.mode == "join" then
		Client.terrain_sync:load_sector(sector)
	end
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
	local sectors = Program:get_sectors()
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

--- Unloads a sector without saving.
-- @param self SectorManager.
-- @param sector Sector ID.
SectorManager.unload_sector = function(self, sector)
	self.sectors[sector] = nil
	self.loaders[sector] = nil
	Program:unload_sector{sector = sector}
end

--- Unloads the world without saving.
-- @param self SectorManager.
SectorManager.unload_world = function(self)
	Program:unload_world()
	self.loaders = {}
	self.sectors = {}
end

--- Unloads sectors that have been inactive long enough.
-- @param self SectorManager.
-- @param secs Seconds since the last update.
SectorManager.update = function(self, secs)
	-- Update sector loaders.
	for k,v in pairs(self.loaders) do
		if not v:update(secs) then
			self.loaders[k] = nil
		end
	end
	-- Reduce the unload frequency.
	if not self.unload_time then return end
	self.update_timer = self.update_timer + secs
	if self.update_timer < 0.2 then return end
	self.update_timer = 0
	-- Unload unused sectors.
	local written = 0
	for k,d in pairs(Program:get_sectors()) do
		if d > self.unload_time and written < 10 then
			-- Save the sector.
			if self.database then
				self:wait_sector_load(k)
				if written == 0 then self.database:query("BEGIN TRANSACTION;") end
				written = written + 1
				self:save_sector(k)
			end
			-- Unload the sector.
			self:unload_sector(k)
		end
	end
	-- Finish the transaction.
	if written > 0 then self.database:query("END TRANSACTION;") end
end

--- Waits for a sector to finish loading.
-- @param self SectorManager.
-- @param sector Sector ID.
SectorManager.wait_sector_load = function(self, sector)
	local loader = self.loaders[sector]
	if not loader then return end
	loader:finish()
	self.loaders[sector] = nil
end

return SectorManager
