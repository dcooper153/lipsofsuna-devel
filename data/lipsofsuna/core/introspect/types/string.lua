return {
	name = "string",
	equals = function(self, val1, val2)
		return val1 == val2
	end,
	validate = function(self, val)
		return type(val) == "string"
	end,
	write_str = function(self, val)
		if string.match(val, "\n") and not string.match(val, "]]") then
			return string.format("[[%s]]", val)
		else
			return string.format("%q", val)
		end
	end}
