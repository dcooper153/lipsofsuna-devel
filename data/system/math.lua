if not Los.program_load_extension("math") then
	error("loading extension `math' failed")
end

------------------------------------------------------------------------------

Aabb = require("system/math/aabb")
Vector = require("system/math/vector")
Quaternion = require("system/math/quaternion")
