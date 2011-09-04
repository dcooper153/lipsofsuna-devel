if not Los.program_load_extension("wireframe") then
	error("loading extension `wireframe' failed")
end

Wireframe = Class()
Wireframe.class_name = "Wireframe"

Wireframe.on = function(self)
	return Los.Wireframe_on()

end