--- Time and date functions.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.time
-- @alias Time

local Class = require("system/class")

if not Los.program_load_extension("time") then
	error("loading extension `time' failed")
end

--- Time and date functions.
-- @type Time
local Time = Class("Time")

--- Gets the current date as an ISO 8601 string.
-- @param self Time class.
-- @return String.
Time.get_date = function(self)
	return Los.time_get_date()
end

--- Gets the current date and time as an ISO 8601 string.
-- @param self Time class.
-- @return String.
Time.get_date_time = function(self)
	return Los.time_get_date_time()
end

--- Gets the number of seconds the program has been running.
-- @param self Program class.
-- @return Number.
Time.get_secs = function(self)
	return Los.program_get_time()
end

--- Gets the current time as an ISO 8601 string.
-- @param self Time class.
-- @return String.
Time.get_time = function(self)
	return Los.time_get_time()
end

return Time
