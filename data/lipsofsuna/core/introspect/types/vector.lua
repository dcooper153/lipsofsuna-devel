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
	read_json = function(self, val)
		if type(val) ~= "table" then return end
		if val.class == Vector then return val end
		return Vector(val[1], val[2], val[3])
	end,
	write_json = function(self, val)
		return {val.x, val.y, val.z}
	end,
	write_str = function(self, val)
		return tostring(val)
	end}
