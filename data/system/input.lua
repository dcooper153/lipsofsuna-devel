require "system/class"

if not Los.program_load_extension("input") then
	error("loading extension `input' failed")
end

------------------------------------------------------------------------------

--- Toggles pointer grabbing.
-- @name Program.cursor_grabbed
-- @class table

--- The current cursor position.
-- @name Program.cursor_position
-- @class table

--- The mouse button mask.
-- @name Program.mouse_button_state
-- @class table

Program:add_class_getters{
	cursor_grabbed = function(s) return Los.input_get_pointer_grab() end,
	cursor_position = function(s) return Class.new(Vector, {handle = Los.input_get_cursor_pos()}) end,
	mouse_button_state = function(s) return Los.input_get_mouse_button_state() end}

Program:add_class_setters{
	cursor_grabbed = function(s, v) Los.input_set_pointer_grab(v) end}
