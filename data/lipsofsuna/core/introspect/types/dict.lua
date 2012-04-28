Introspect.types_dict["dict"] = {
	equals = function(val1, val2)
		for k,v in pairs(val1) do
			if v ~= val2[k] then return end
		end
		for k,v in pairs(val2) do
			if v ~= val1[k] then return end
		end
		return true
	end,
	validate = function(val, field)
		local t = Introspect.types_dict[field.dict.type]
		for k,v in pairs(val) do
			if type(k) ~= "string" then return end
			if t.validate and not t.validate(v, field.dict) then return end
		end
		return true
	end,
	write_str = function(val, field)
		local t = Introspect.types_dict[field.dict.type]
		local lst = {}
		for k,v in pairs(val) do
			table.insert(lst, {k, v})
		end
		table.sort(lst, function(a,b) return a[1] < b[1] end)
		local buf = {"{"}
		for k,v in ipairs(lst) do
			if k > 1 then
				table.insert(buf, ", ")
			end
			table.insert(buf, string.format("[%q] = %s", v[1], t.write_str(v[2], field.dict)))
		end
		table.insert(buf, "}")
		return table.concat(buf)
	end}
