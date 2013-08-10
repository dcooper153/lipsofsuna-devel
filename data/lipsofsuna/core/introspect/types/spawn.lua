return {
	name = "spawn",
	equals = function(self, val1, val2)
		-- TODO
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
