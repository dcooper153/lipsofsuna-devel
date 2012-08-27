--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.log
-- @alias Log

local Class = require("system/class")
local Time = require("system/time")

--- TODO:doc
-- @type Log
local Log = Class("Log")

Log.new = function(clss)
	return Class.new(clss)
end

Log.format = function(self, format, ...)
	print(Time:get_date_time() .. ": " .. string.format(format, ...))
end

Log.text = function(self, text)
	print(Time:get_date_time() .. ": " .. text)
end

return Log


