return {
	name = "boolean",
	equals = function(self, val1, val2)
		return val1 == val2
	end,
	validate = function(self, val)
		return type(val) == "boolean"
	end,
	write_str = function(self, val)
		return tostring(val)
	end}
