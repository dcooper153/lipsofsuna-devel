--- Radio menu widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.widgets.radio_menu
-- @alias UiRadioMenu

local Class = require("system/class")
local UiMenu = require("ui/widgets/menu")

--- Radio menu widget.
-- @type UiRadioMenu
local UiRadioMenu = Class("UiRadioMenu", UiMenu)

--- Creates a new selector widget.
-- @param clss UiRadioMenu class.
-- @param label Label.
-- @param populate Function for populating the menu.
-- @return UiRadioMenu.
UiRadioMenu.new = function(clss, label, populate)
	local self = UiMenu.new(clss)
	self.label = label
	self.__populate = populate
	return self
end

--- Handles the apply command.
-- @param self UiMenu.
UiRadioMenu.apply = function(self)
	if self.__populate then
		self:__populate()
	end
	UiMenu.apply(self)
end

--- Adds a radio button to the menu.
-- @param self UiRadioMenu.
-- @param label Label of the button.
-- @param active True if the item is the active one.
-- @param func Callback function.
UiRadioMenu.add_item = function(self, label, active, func)
	local widget = Widgets.Uiradio(label, "menu", function(w)
		if func then func(w) end
		self:set_menu_opened(false)
	end)
	widget.value = active
	self:add_widget(widget)
end

--- Sets the items of the menu.
-- @param self UiRadioMenu.
-- @param value List of {label,function} items.
-- @param active Index of the active radio button.
UiRadioMenu.set_menu_items = function(self, value, active)
	self:clear()
	for k,v in ipairs(value) do
		self:add_item(v[1], v[2], k == active)
	end
end

--- Rebuilds the canvas.
-- @param self UiRadioMenu.
UiRadioMenu.rebuild_canvas = function(self)
	Theme:draw_button(self, self.label,
		0, 0, self.size.x, self.size.y, self.focused, false)
end

return UiRadioMenu
