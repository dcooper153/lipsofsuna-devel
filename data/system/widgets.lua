--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.widgets
-- @alias Widgets

local Class = require("system/class")

if not Los.program_load_extension("widgets") then
	error("loading extension `widgets' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Widgets
Widgets = Class("Widgets")

--- Adds a font style.
-- @param clss Widgets class.
-- @param name Font name.
-- @param file Font file.
-- @param size Font size.
Widgets.add_font_style = function(clss, name, file, size)
	Los.widgets_add_font_style(name, file, size)
end

--- Finds a widget under the cursor that has the given member.
-- @param clss Widgets class.
-- @param handler Member name.
-- @return Widget, or nil.
Widgets.find_handler_widget = function(clss, handler)
	local w = clss:find_widget()
	while w do
		if w[handler] then return w end
		w = w:get_parent()
	end
end

--- Finds a widget by screen position.
-- @param clss Widgets class.
-- @param args Arguments.<ul>
--   <li>1,point: Screen position.</li></ul>
Widgets.find_widget = function(clss, args)
	local handle = Los.widgets_find_widget(args)
	if not handle then return end
	return __userdata_lookup[handle]
end

--- Gets the currently focused widget.
-- @param self Widgets class.
-- @return Widget, or nil.
Widgets.get_widget_under_cursor = function(self)
	local handle = Los.widgets_get_focused_widget()
	if not handle then return end
	return __userdata_lookup[handle]
end

return Widgets


