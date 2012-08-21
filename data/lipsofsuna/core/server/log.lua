local Class = require("system/class")
local Time = require("system/time")

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
