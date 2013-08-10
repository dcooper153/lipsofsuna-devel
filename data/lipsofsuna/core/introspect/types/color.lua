return {
	name = "color",
	equals = function(self, val1, val2)
		return val1[1] == val2[1] and
		       val1[2] == val2[2] and
		       val1[3] == val2[3] and
		       val1[4] == val2[4]
	end,
	validate = function(self, val)
		return type(val) == "table" and
			type(val[1]) == "number" and
			type(val[2]) == "number" and
			type(val[3]) == "number" and
			(type(val[4]) == "number" or type(val[4]) == "nil")
	end,
	write_str = function(self, val)
		local write = function(i)
			return self.types_dict["number"].write_str(self, val[i])
		end
		if val[4] then
			return string.format("{%s,%s,%s,%s}", write(1), write(2), write(3), write(4))
		else
			return string.format("{%s,%s,%s}", write(1), write(2), write(3))
		end
	end}
