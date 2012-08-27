--- Implements asynchronous sector generation.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.generator.sector_generator
-- @alias SectorGenerator

local Class = require("system/class")

--- Generates sector data asynchronously.
-- @type SectorGenerator
local SectorGenerator = Class("SectorGenerator")

--- Creates a new sector generator.
-- @param clss SectorGenerator class.
-- @param generator Generator.
-- @param sector Sector ID.
-- @return SectorGenerator.
SectorGenerator.new = function(clss, generator, sector)
	local self = Class.new(clss)
	self.routine = coroutine.create(function()
		self:execute(generator, sector, coroutine.yield)
	end)
	return self
end

--- Executes the generation process.<br/>
--
-- This can be called either synchronously or asynchronously by providing
-- the desired yield function. When run in a coroutine, coroutine.yield()
-- should be used. When running synchronously, a dummy function should be
-- used.
--
-- @param clss SectorGenerator class.
-- @param generator Generator.
-- @param sector Sector ID.
-- @param yield Function.
SectorGenerator.execute = function(clss, generator, sector, yield)
	generator.sector_types.Main:generate(sector, yield)
	yield()
end

--- Forces the generator to run until finished.
-- @param self SectorGenerator.
SectorGenerator.finish = function(self)
	repeat until not self:update(1)
	self.routine = nil
end

--- Updates the sector generator.
-- @param eslf SectorGenerator.
-- @param secs Seconds since the last update.
-- @return True if still loading, false if finished.
SectorGenerator.update = function(self, secs)
	if not self.routine then return end
	local ret,err = coroutine.resume(self.routine, secs)
	if not ret then print(debug.traceback(self.routine, err)) end
	if coroutine.status(self.routine) == "dead" then
		self.routine = nil
		return
	end
	return true
end

return SectorGenerator
