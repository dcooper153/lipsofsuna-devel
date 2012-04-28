Introspect.types_dict["boolean"] = {
	equals = function(val1, val2)
		return val1 == val2
	end,
	validate = function(val)
		return type(val) == "boolean"
	end,
	write_str = function(val)
		return tostring(val)
	end}
