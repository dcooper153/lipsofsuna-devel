Log = Class()
Log.class_name = "Log"

Log.format = function(clss, format, ...)
	print(Time.date_time .. ": " .. string.format(format, ...))
end

Log.text = function(clss, text)
	print(Time.date_time .. ": " .. text)
end
