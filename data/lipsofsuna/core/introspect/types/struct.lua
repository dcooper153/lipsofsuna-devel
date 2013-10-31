return {
	name = "struct",
	equals = function(self, val1, val2)
		for k,v in pairs(val1) do
			local f = field.struct[k]
			local t = self.types_dict[f.type]
			if not t.equals(self, val2[k]) then return end
		end
		for k,v in pairs(val2) do
			local f = field.struct[k]
			local t = self.types_dict[f.type]
			if not t.equals(self, val1[k]) then return end
		end
		return true
	end,
	validate = function(self, val, field)
		for k,v in pairs(val) do
			if type(k) ~= "string" then return end
			local f = field.struct[k]
			if not f then return end
			local t = self.types_dict[f.type]
			if t.validate and not t.validate(self, v, f) then return end
		end
		return true
	end,
	read_json = function(self, val, field)
		if not val then return end
		local res = {}
		for k,v in pairs(val) do
			local f = field.struct[k]
			local t = self.types_dict[f.type]
			if t.read_json then
				res[k] = t.read_json(self, v)
			else
				res[k] = v
			end
		end
		return res
	end,
	write_json = function(self, val, field)
		if not val then return end
		local res = {}
		for k,v in pairs(val) do
			local f = field.struct[k]
			local t = self.types_dict[f.type]
			if t.write_json then
				res[k] = t.write_json(self, v)
			else
				res[k] = v
			end
		end
		return res
	end,
	write_str = function(self, val, field)
		local lst = {}
		for k,v in pairs(val) do
			table.insert(lst, {k, v})
		end
		table.sort(lst, function(a,b) return a[1] < b[1] end)
		local buf = {"{"}
		for k,v in pairs(lst) do
			if k > 1 then
				table.insert(buf, ", ")
			end
			local f = field.struct[k]
			local t = self.types_dict[f.type]
			table.insert(buf, string.format("[%q] = %s", v[1], t.write_str(self, v[2], f)))
		end
		table.insert(buf, "}")
		return table.concat(buf)
	end}
