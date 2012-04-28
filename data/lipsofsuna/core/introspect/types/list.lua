Introspect.types_dict["list"] = {
	equals = function(val1, val2)
		local len1 = #val1
		local len2 = #val2
		if len1 ~= len2 then return end
		for k,v in ipairs(val1) do
			if v ~= val2[k] then return end
		end
		return true
	end,
	validate = function(val, field)
		local t = Introspect.types_dict[field.list.type]
		for k,v in ipairs(val) do
			if t.validate and not t.validate(v, field.list) then return end
		end
		return true
	end,
	write_str = function(val, field)
		local t = Introspect.types_dict[field.list.type]
		local buf = {"{"}
		for k,v in ipairs(val) do
			if k > 1 then
				table.insert(buf, ",")
			end
			table.insert(buf, t.write_str(v, field.list))
		end
		table.insert(buf, "}")
		return table.concat(buf)
	end}
