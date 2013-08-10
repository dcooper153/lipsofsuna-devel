return {
	name = "generic",
	equals = function(self, val1, val2)
		-- TODO
	end,
	write_str = function(self, val)
		local t = type(val)
		if t == "table" then
			if val.class then
				return tostring(val)
			else
				return self.types_dict["table"].write_str(self, val)
			end
		elseif t == "string" then
			return self.types_dict["string"].write_str(self, val)
		elseif t == "function" then
			return "function() end"
		else
			return tostring(val)
		end
	end}
