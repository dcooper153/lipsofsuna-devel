local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("input") then
	error("loading extension `input' failed")
end

------------------------------------------------------------------------------

local Input = Class("Input")

--- Gets the mouse button state mask.
-- @param self Input class.
-- @return Number.
Input.get_mouse_button_state = function(self)
	return Los.input_get_mouse_button_state()
end

--- Gets the pointer grabbing state.
-- @param self Input class.
-- @return True if grabbed, false if not.
Input.get_pointer_grab = function(self)
	return Los.input_get_pointer_grab()
end

--- Enables or disables pointer grabbing.
-- @param self Input class.
-- @param v True to grab, false to ungrab.
Input.set_pointer_grab = function(self, v)
	Los.input_set_pointer_grab(v)
end

--- Gets the current cursor position.
-- @param self Input class.
-- @return Vector.
Input.get_pointer_position = function(self)
	return Vector:new_from_handle(Los.input_get_cursor_pos())
end

return Input
