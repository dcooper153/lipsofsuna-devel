local Class = require("system/class")

if not Los.program_load_extension("wireframe") then
	error("loading extension `wireframe' failed")
end

------------------------------------------------------------------------------

local Wireframe = Class("Wireframe")

Wireframe.on = function(self)
	return Los.Wireframe_on()
end

return Wireframe
