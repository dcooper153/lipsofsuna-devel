Introspect.types_dict["spawn"] = {
	equals = function(val1, val2)
		-- TODO
	end,
	write_str = function(val)
		local s1 = Introspect.types_dict["string"].write_str(val[1])
		local s2 = Introspect.types_dict["vector"].write_str(val[2])
		local fields = {s1, s2}
		local str = "{"
		for k,v in ipairs(fields) do
			if k > 1 then
				str = string.format("%s, %s", str, v)
			else
				str = string.format("%s%s", str, v)
			end
		end
		return str .. "}"
	end}
