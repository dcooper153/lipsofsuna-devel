Introspect.types_dict["map tile list"] = {
	equals = function(val1, val2)
		local len1 = #val1
		local len2 = #val2
		if len1 ~= len2 then return end
		local t = Introspect.types_dict["map tile"]
		for k,v in ipairs(val1) do
			if not t.equals(val2[k], v) then return end
		end
		return true
	end,
	write_str = function(val)
		if #val == 0 then return end
		local t = Introspect.types_dict["map tile"]
		local y = -1
		local z = -1
		local comma
		local buf = {"{\n"}
		for k,v in ipairs(val) do
			if comma then
				table.insert(buf, (y ~= v[2] or z ~= v[3]) and ",\n\t\t" or ", ")
			else
				table.insert(buf, "\t\t")
				comma = true
			end
			y,z = v[2],v[3]
			table.insert(buf, t.write_str(v))
		end
		table.insert(buf, "}")
		return table.concat(buf)
	end}
