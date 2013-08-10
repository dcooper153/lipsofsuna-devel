return {
	name = "map tile",
	equals = function(self, val1, val2)
		return val1[1] == val2[1] and
		       val1[2] == val2[2] and
		       val1[3] == val2[3] and
		       val1[4] == val2[4]
	end,
	write_str = function(self, val)
		return string.format("{%d,%d,%d,%q}",
			val[1], val[2], val[3], val[4])
	end}
