if not Program:load_extension("widgets") then
	error("loading extension `widgets' failed")
end

local widget_getters = {
	behind = function(s) return s:get_behind() end,
	cols = function(s) return s:get_cols() end,
	floating = function(s) return s:get_floating() end,
	fullscreen = function(s) return s:get_fullscreen() end,
	height = function(s) return s:get_height() end,
	margins = function(s) return s:get_margins() end,
	parent = function(s) return s:get_parent() end,
	rows = function(s) return s:get_rows() end,
	spacings = function(s) return s:get_spacings() end,
	visible = function(s) return s:get_visible() end,
	width = function(s) return s:get_width() end,
	x = function(s) return s:get_x() end,
	y = function(s) return s:get_y() end}

local widget_setters = {
	behind = function(s, v) s:set_behind(v) end,
	cols = function(s, v) s:set_cols(v) end,
	floating = function(s, v) s:set_floating(v) end,
	fullscreen = function(s, v) s:set_fullscreen(v) end,
	margins = function(s, v) s:set_margins(v) end,
	rows = function(s, v) s:set_rows(v) end,
	spacings = function(s, v) s:set_spacings(v) end,
	visible = function(s, v) s:set_visible(v) end,
	x = function(s, v) s:set_x(v) end,
	y = function(s, v) s:set_y(v) end}

Widget.getter = function(self, key)
	local widgetgetterfunc = widget_getters[key]
	if widgetgetterfunc then return widgetgetterfunc(self) end
	return Class.getter(self, key)
end

Widget.setter = function(self, key, value)
	local widgetsetterfunc = widget_setters[key]
	if widgetsetterfunc then return widgetsetterfunc(self, value) end
	return Class.setter(self, key, value)
end

Widgets.getter = function(self, key)
	if key == "focused_widget" then return self:get_focused_widget() end
	return Class.getter(self, key)
end

require "system/widgets/button"
require "system/widgets/combo"
require "system/widgets/cursor"
require "system/widgets/entry"
require "system/widgets/frame"
require "system/widgets/menuitem"
require "system/widgets/widgets"
