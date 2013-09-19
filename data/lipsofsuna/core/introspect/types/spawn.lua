return {
	name = "spawn",
	equals = function(self, val1, val2)
		return self.types_dict["string"].equals(self, val1[1], val2[1]) and
		       self.types_dict["vector"].equals(self, val1[2], val2[2])
	end,
	validate = function(self, val, field)
		if type(val) ~= "table" then return end
		return self.types_dict["string"].validate(self, val[1]) and
		       self.types_dict["vector"].validate(self, val[2])
	end,
	read_json = function(self, val)
		if type(val) ~= "table" then return end
		return {val[1], self.types_dict["vector"].read_json(self, val[2])}
	end,
	write_json = function(self, val)
		return {val[1], self.types_dict["vector"].write_json(self, val[2])}
	end,
	write_str = function(self, val)
		local s1 = self.types_dict["string"].write_str(self, val[1])
		local s2 = self.types_dict["vector"].write_str(self, val[2])
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
