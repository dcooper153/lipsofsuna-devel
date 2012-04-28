Introspect.types_dict["string"] = {
	equals = function(val1, val2)
		return val1 == val2
	end,
	validate = function(val)
		return type(val) == "string"
	end,
	write_str = function(val)
		if string.match(val, "\n") and not string.match(val, "]]") then
			return string.format("[[%s]]", val)
		else
			return string.format("%q", val)
		end
	end}
