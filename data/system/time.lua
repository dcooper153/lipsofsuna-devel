local Class = require("system/class")

if not Los.program_load_extension("time") then
	error("loading extension `time' failed")
end

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

--- Gets the current time as an ISO 8601 string.
-- @param self Time class.
-- @return String.
Time.get_time = function(self)
	return Los.time_get_time()
end

return Time
