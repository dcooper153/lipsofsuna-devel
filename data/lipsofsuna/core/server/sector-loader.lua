--- Implements asynchronous sector loading.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.sector_loader
-- @alias SectorLoader

local Class = require("system/class")
local SectorGenerator = require("core/generator/sector-generator")

--- Loads sector data asynchronously.
-- @type SectorLoader
local SectorLoader = Class("SectorLoader")

--- Creates a new sector loader.
-- @param clss SectorLoader class.
-- @param manager SectorManager.
-- @param sector Sector ID.
-- @return SectorLoader.
SectorLoader.new = function(clss, manager, sector)
	local self = Class.new(clss)
	self.routine = coroutine.create(function()
		self:execute(manager, sector, coroutine.yield)
	end)
	return self
end

--- Executes the loading process.<br/>
--
-- This can be called either synchronously or asynchronously by providing
-- the desired yield function. When run in a coroutine, coroutine.yield()
-- should be used. When running synchronously, a dummy function should be
-- used.
--
-- @param clss SectorLoader class.
-- @param manager SectorManager.
-- @param sector Sector ID.
-- @param yield Function.
SectorLoader.execute = function(clss, manager, sector, yield)
	-- Load terrain.
	local rows = manager.database:query("SELECT * FROM terrain WHERE sector=?;", {sector})
	local terrain = #rows ~= 0
	yield()
	-- Generate terrain.
	if terrain then
		for k,v in ipairs(rows) do
			Voxel:paste_region{sector = sector, packet = v[2]}
			yield()
		end
	else
		SectorGenerator:execute(Server.generator, sector, yield)
		yield()
	end
	-- Load objects.
	local objects = Server.object_database:load_sector_objects(sector)
	yield()
	-- Trigger global events.
	Server.events:sector_created(sector, terrain, objects)
	yield()
end

--- Forces the loader to run until finished.
-- @param self SectorLoader.
SectorLoader.finish = function(self)
	repeat until not self:update(1)
	self.routine = nil
end

--- Updates the sector loader.
-- @param self SectorLoader.
-- @param secs Seconds since the last update.
-- @return True if still loading, false if finished.
SectorLoader.update = function(self, secs)
	if not self.routine then return end
	local ret,err = coroutine.resume(self.routine, secs)
	if not ret then print(debug.traceback(self.routine, err)) end
	if coroutine.status(self.routine) == "dead" then
		self.routine = nil
		return
	end
	return true
end

return SectorLoader
