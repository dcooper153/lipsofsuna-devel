--- Logging.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module main.log
-- @alias Log

local Class = require("system/class")
local Time = require("system/time")

--- Logging.
-- @type Log
local Log = Class("Log")

--- Creates a new log.
-- @param clss Log class.
-- @return Log.
Log.new = function(clss)
	return Class.new(clss)
end

--- Appends a formatted log message.
-- @param self Log.
-- @param format Format string.
-- @param ... Format arguments.
Log.format = function(self, format, ...)
	print(Time:get_date_time() .. ": " .. string.format(format, ...))
end

--- Appends a log message.
-- @param self Log.
-- @param text String.
Log.text = function(self, text)
	print(Time:get_date_time() .. ": " .. text)
end

return Log
