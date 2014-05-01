--- Implements trivial timing and profiling.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.main.profiling
-- @alias ModelManager

local Class = require("system/class")

--- Implements trivial timing and profiling.
-- @type ModelManager
local Timing = Class("Timing")

--- Creates a new timing.
-- @param clss Timing class.
-- @return Timing.
Timing.new = function(clss)
	local self = Class.new(clss)
	self.action_times = {}
	self.frame_time = 0.0001
	return self
end

--- Resets profiling data.
-- @param self Timing.
Timing.reset_profiling = function(self)
	self.action_times = {}
end

--- Starts an action.
-- @param self Timing.
-- @param name Action name.
Timing.start_action = function(self, name)
	local time = Program:get_time()
	-- End the previous action.
	if self.__action_start then
		local name = self.__action_name
		local delta = time - self.__action_start
		local action = self.action_times[name] or {delta, delta, delta, 0, 0}
		action[1] = delta
		action[2] = math.min(action[2], delta)
		action[3] = math.max(action[3], delta)
		action[4] = action[4] + delta
		action[5] = action[5] + 1
		self.action_times[name] = action
	end
	-- Start the next action.
	self.__action_name = name
	self.__action_start = time
end

--- Starts a frame.
-- @param self Timing.
Timing.start_frame = function(self)
	local time = Program:get_time()
	if self.__frame_start then
		self.frame_time = time - self.__frame_start
	end
	self.__frame_start = time
end

--- Gets the FPS of the last frame.
-- @param self Timing.
-- @return FPS.
Timing.get_fps = function(self)
	return self.frame_time > 0 and 1.0 / self.frame_time or 0
end

--- Gets the last frame duration.
-- @param self Timing.
-- @return Time in seconds.
Timing.get_frame_duration = function(self)
	return self.frame_time
end

--- Gets the accumulated profiling data.<br/>
--
-- This function returns the last, best, worst and average action timings
-- that have occurred during the execution of the game. The return value
-- is a list consisting of {name, last, best, worst, average} tables.
--
-- @param self Timing.
-- @return List of tables.
Timing.get_profiling_data = function(self)
	local res = {}
	for k,v in pairs(self.action_times) do
		table.insert(res, {k, v[1], v[2], v[3], v[4] / v[5]})
	end
	table.sort(res, function(a,b) return a[1] < b[1] end)
	return res
end

--- Gets the accumulated profiling data in a string form.
-- @param self Timing.
-- @return String.
Timing.get_profiling_string = function(self)
	local res = {}
	for k,v in ipairs(self:get_profiling_data()) do
		table.insert(res, string.format("%9s: %6d %6d %6d %6d ms",
			v[1], v[2] * 1000, v[3] * 1000, v[4] * 1000, v[5] * 1000))
	end
	return string.format("%9s  %6s %6s %6s %6s\n", "Action", "Last", "Best", "Worst", "Average") ..
	       table.concat(res, "\n")
end

return Timing
