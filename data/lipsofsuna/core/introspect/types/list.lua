return {
	name = "list",
	equals = function(self, val1, val2)
		local len1 = #val1
		local len2 = #val2
		if len1 ~= len2 then return end
		for k,v in ipairs(val1) do
			if v ~= val2[k] then return end
		end
		return true
	end,
	validate = function(self, val, field)
		local t = self.types_dict[field.list.type]
		for k,v in ipairs(val) do
			if t.validate and not t.validate(self, v, field.list) then return end
		end
		return true
	end,
	read_json = function(self, val, field)
		if not val then return end
		local t = self.types_dict[field.list.type]
		if not t.read_json then return val end
		local res = {}
		for k,v in ipairs(val) do
			res[k] = t.read_json(self, v)
		end
		return res
	end,
	write_json = function(self, val, field)
		if not val then return end
		local t = self.types_dict[field.list.type]
		if not t.write_json then return val end
		local res = {}
		for k,v in ipairs(val) do
			res[k] = t.write_json(self, v)
		end
		return res
	end,
	write_str = function(self, val, field)
		local t = self.types_dict[field.list.type]
		local buf = {"{"}
		for k,v in ipairs(val) do
			if k > 1 then
				table.insert(buf, ",")
			end
			table.insert(buf, t.write_str(self, v, field.list))
		end
		table.insert(buf, "}")
		return table.concat(buf)
	end}
