return {
	name = "map object list",
	equals = function(self, val1, val2)
		local len1 = #val1
		local len2 = #val2
		if len1 ~= len2 then return end
		local t = self.types_dict["map object"]
		for k,v in ipairs(val1) do
			if not t.equals(self, val2[k], v) then return end
		end
		return true
	end,
	write_str = function(self, val)
		if #val == 0 then return end
		local t = self.types_dict["map object"]
		local buf = {"{\n"}
		for k,v in ipairs(val) do
			if k == 1 then
				table.insert(buf, "\t\t")
			else
				table.insert(buf, ",\n\t\t")
			end
			table.insert(buf, t.write_str(self, v))
		end
		table.insert(buf, "}")
		return table.concat(buf)
	end}
