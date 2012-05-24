if not Los.program_load_extension("time") then
	error("loading extension `time' failed")
end

Time = Class()
Time.class_name = "Time"

--- The current date as an ISO 8601 string.
-- @name Time.date
-- @class table

--- The current date and time as an ISO 8601 string.
-- @name Time.date_time
-- @class table

--- The current time as an ISO 8601 string.
-- @name Time.time
-- @class table

Time:add_class_getters{
	date = function(self) return Los.time_get_date() end,
	date_time = function(self) return Los.time_get_date_time() end,
	time = function(self) return Los.time_get_time() end}

