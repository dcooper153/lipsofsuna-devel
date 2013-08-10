local Vector = require("system/math/vector")

return {
	name = "vector",
	equals = function(self, val1, val2)
		return val1.x == val2.x and
		       val1.y == val2.y and
		       val1.z == val2.z
	end,
	validate = function(self, val)
		return type(val) == "table" and val.class == Vector
	end,
	write_str = function(self, val)
		return tostring(val)
	end}
