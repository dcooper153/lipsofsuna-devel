--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module common.sectors
-- @alias Sectors

local Class = require("system/class")
local Timer = require("system/timer")

--- TODO:doc
-- @type Sectors
Sectors = Class("Sectors")

--- Creates a new sector manager.
-- @param clss Sectors class.
-- @param database Database, or nil.
-- @return Sectors.
Sectors.new = function(clss, database)
	local self = Class.new(clss)
	self.database = database
	self.sectors = {}
	self.unload_time = 10
	-- Initialize the database tables needed by us.
	if self.database then
		self.database:query("CREATE TABLE IF NOT EXISTS objects (id INTEGER PRIMARY KEY,sector UNSIGNED INTEGER,data TEXT);");
		self.database:query("CREATE TABLE IF NOT EXISTS terrain (sector INTEGER PRIMARY KEY,data BLOB);");
	end
	-- Load and unload sectors automatically from now on.
	self.timer = Timer{delay = 2, func = function() self:update() end}
	return self
end

--- Called when a sector is created by the game.
-- @param self Sectors.
-- @param sector Sector index.
-- @param terrain True if terrain was loaded.
-- @param objects Array of objects.
Sectors.created_sector = function(self, sector, terrain, objects)
	if not Server.initialized then return end
	-- Create fractal terrain for newly found sectors.
	if not terrain then Server.generator:generate_sector(sector) end
	-- Spawn monsters and events.
	Server.events:sector_created(sector, terrain, objects)
end

--- Reads a sector from the database.
-- @param self Sectors.
-- @param sector Sector index.
Sectors.load_sector = function(self, sector)
	local objects = {}
	local terrain = nil
	-- Only load once.
	if self.sectors[sector] then return end
	self.sectors[sector] = true
	-- Load database content.
	if self.database then
		-- Load terrain.
		local rows = self.database:query("SELECT * FROM terrain WHERE sector=?;", {sector})
		if #rows ~= 0 then
			for k,v in ipairs(rows) do
				Voxel:paste_region{sector = sector, packet = v[2]}
			end
			terrain = true
		end
		-- Load objects.
		if Server.initialized then
			objects = Server.object_database:load_sector_objects(sector)
		end
	end
	-- Load custom content.
	self:created_sector(sector, terrain, objects)
	-- Handle client side terrain swapping.
	if Game.mode == "join" then
		Client.terrain_sync:load_sector(sector)
	end
end

--- Saves a sector to the database.
-- @param self Sectors.
-- @param sector Sector index.
Sectors.save_sector = function(self, sector)
	if not self.database then return end
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
-- @param self Sectors.
-- @param erase True to completely erase the old map.
-- @param progress Progress callback.
Sectors.save_world = function(self, erase, progress)
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

--- Unloads the world without saving.
-- @param self Sectors.
Sectors.unload_world = function(self)
	Program:unload_world()
	self.sectors = {}
end

--- Unloads sectors that have been inactive long enough.
-- @param self Sectors.
Sectors.update = function(self)
	if not self.unload_time then return end
	local written = 0
	for k,d in pairs(Program:get_sectors()) do
		if d > self.unload_time and written < 40 then
			-- Save the sector.
			if self.database then
				if written == 0 then self.database:query("BEGIN TRANSACTION;") end
				written = written + 1
				self:save_sector(k)
			end
			-- Unload the sector.
			self.sectors[k] = nil
			Program:unload_sector{sector = k}
		end
	end
	-- Finish the transaction.
	if written > 0 then self.database:query("END TRANSACTION;") end
end


