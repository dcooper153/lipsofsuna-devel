require "system/class"
require "system/widget"

if not Los.program_load_extension("widgets") then
	error("loading extension `widgets' failed")
end

------------------------------------------------------------------------------

--- Adds a font style.
-- @param clss Widgets class.
-- @param ... Arguments.<ul>
--   <li>1,name: Font name.</li>
--   <li>2,file: Font file.</li>
--   <li>3,size: Font size.</li></ul>
Widgets.add_font_style = function(clss, ...)
	Los.widgets_add_font_style(...)
end

--- Draws the user interface.
-- @param clss Widgets class.
Widgets.draw = function(clss)
	Los.widgets_draw()
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

--- Currently focused widget.
-- @name Widgets.focused_widget
-- @class table
Widgets.class_getters = {
	widget_under_cursor = function(s, k)
		local handle = Los.widgets_get_focused_widget()
		if not handle then return end
		return __userdata_lookup[handle]
	end}

------------------------------------------------------------------------------

require "system/widgets/button"
require "system/widgets/combo"
require "system/widgets/cursor"
require "system/widgets/entry"
require "system/widgets/frame"
require "system/widgets/list"
require "system/widgets/menuitem"
require "system/widgets/scrollbar"
require "system/widgets/title"
require "system/widgets/toggle"
require "system/widgets/widgets"
