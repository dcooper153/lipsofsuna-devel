Sectors = Class()

--- Initializes the serializer.
-- @param clss Serialize class.
-- @param args Arguments.<ul>
--   <li>database: Database.</li>
--   <li>save_objects: False to disable saving of objects.</li>
--   <li>save_terrain: False to disable saving of terrain.</li>
--   <li>unload_time: Number of seconds it takes for an inactive sector to be unloaded.</li></ul>
-- @return New sectors.
Sectors.new = function(clss, args)
	local self = Class.new(clss, args)
	self.sectors = {}
	self.save_objects = (self.save_objects ~= false)
	self.save_terrain = (self.save_terrain ~= false)
	self.unload_time = self.unload_time or 10
	-- Use asynchronous writes for much better write performance.
	self.database:query("PRAGMA synchronous=OFF;")
	self.database:query("PRAGMA count_changes=OFF;")
	-- Initialize the database tables needed by us.
	if self.save_objects then
		self.database:query("CREATE TABLE IF NOT EXISTS objects (id INTEGER PRIMARY KEY,sector UNSIGNED INTEGER,data TEXT);");
	end
	if self.save_terrain then
		self.database:query("CREATE TABLE IF NOT EXISTS terrain (sector INTEGER PRIMARY KEY,data BLOB);");
	end
	-- Load and unload sectors automatically from now on.
	self.handler = Eventhandler{type = "sector-load", func = function(_,a) self:load_sector(a.sector) end}
	self.timer = Timer{delay = 2, func = function() self:update() end}
	return self
end

--- Called when a sector is created by the game.
-- @param self Sectors.
-- @param sector Sector index.
-- @param terrain True if terrain was loaded.
-- @param objects Array of objects.
Sectors.created_sector = function(self, sector, terrain, objects)
end

--- Removes all sectors from the database.
-- @param self Sectors.
Sectors.erase_world = function(self, erase)
	self.database:query("BEGIN TRANSACTION;")
	if self.save_objects then self.database:query("DELETE FROM objects;") end
	if self.save_terrain then self.database:query("DELETE FROM terrain;") end
	self.database:query("END TRANSACTION;")
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
	-- Load terrain.
	if self.save_terrain then
		local rows = self.database:query("SELECT * FROM terrain WHERE sector=?;", {sector})
		if #rows ~= 0 then
			for k,v in ipairs(rows) do
				Voxel:paste_region{sector = sector, packet = v[2]}
			end
			terrain = true
		end
	end
	-- Load objects. Since tiles are loaded in background, we need to wait for them to be
	-- loaded before creation the object or else the object will fall inside the ground.
	-- TODO: Should use a tiles-loaded event to determine when the tiles have been loaded.
	if self.save_objects then
		local rows = self.database:query("SELECT * FROM objects WHERE sector=?;", {sector})
		Timer{delay = 1, func = function(timer)
			for k,v in ipairs(rows) do
				local func = assert(loadstring("return function()\n" .. v[3] .. "\nend"))()
				if func then
					local object = func()
					if object then object.realized = true end
					table.insert(objects, object)
				end
			end
			timer:disable()
			self:created_sector(sector, terrain, objects)
		end}
	else
		Timer{delay = 1, func = function(timer)
			timer:disable()
			self:created_sector(sector, terrain, objects)
		end}
	end
end

--- Saves a sector to the database.
-- @param self Sectors.
-- @param sector Sector index.
Sectors.save_sector = function(self, sector)
	-- Write objects.
	if self.save_objects then
		self.database:query("DELETE FROM objects WHERE sector=?;", {sector})
		local objs = Object:find{sector = sector}
		for k,v in pairs(objs) do
			local data = v:write()
			if data and not Class:check{data = v, name = "Player"} then
				self.database:query("REPLACE INTO objects (id,sector,data) VALUES (?,?,?);", {v.id, sector, data})
			end
		end
	end
	-- Write terrain.
	if self.save_terrain then
		self.database:query("DELETE FROM terrain WHERE sector=?;", {sector})
		local data = Voxel:copy_region{sector = sector}
		self.database:query("INSERT INTO terrain (sector,data) VALUES (?,?);", {sector, data})
	end
end

--- Saves all active sectors to the database.
-- @param self Sectors.
-- @param erase True to completely erase the old map.
Sectors.save_world = function(self, erase)
	self.database:query("BEGIN TRANSACTION;")
	if erase then
		if self.save_objects then self.database:query("DELETE FROM objects;") end
		if self.save_terrain then self.database:query("DELETE FROM terrain;") end
	end
	for k,v in pairs(Program.sectors) do
		self:save_sector(k)
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
	local written = 0
	for k,d in pairs(Program.sectors) do
		if d > self.unload_time and written < 5 then
			-- Group into a single transaction.
			if written == 0 then self.database:query("BEGIN TRANSACTION;") end
			written = written + 1
			-- Save and unload the sector.
			self:save_sector(k)
			self.sectors[k] = nil
			Program:unload_sector{sector = k}
		end
	end
	-- Finish the transaction.
	if written > 0 then self.database:query("END TRANSACTION;") end
end
