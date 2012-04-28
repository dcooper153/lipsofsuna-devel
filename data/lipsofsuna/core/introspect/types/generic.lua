Introspect.types_dict["generic"] = {
	equals = function(val1, val2)
		-- TODO
	end,
	write_str = function(val)
		local t = type(val)
		if t == "table" then
			if val.class then
				return tostring(val)
			else
				return Introspect.types_dict["table"].write_str(val)
			end
		elseif t == "string" then
			return Introspect.types_dict["string"].write_str(val)
		elseif t == "function" then
			return "function() end"
		else
			return tostring(val)
		end
	end}
