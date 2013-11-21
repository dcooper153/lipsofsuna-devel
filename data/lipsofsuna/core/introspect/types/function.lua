return {
	name = "function",
	equals = function(self, val1, val2)
		return true
	end,
	validate = function(self, val)
		return type(val) == "function"
	end,
	read_json = function(self, val)
		if type(val) ~= "string" then return end
		local res,err = loadstring("return " .. val)
		if err then error(err) end
		return res()
	end,
	write_json = function(self, val)
		return tostring(val)
	end,
	write_str = function(self, val)
		return tostring(val)
	end}
