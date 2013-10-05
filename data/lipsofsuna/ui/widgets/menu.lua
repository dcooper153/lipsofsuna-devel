--- Menu widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.widgets.menu
-- @alias UiMenu

local Class = require("system/class")
local InputHandler = require("ui/input-handler")
local UiVBox = require("ui/widgets/vbox")
local UiWidget = require("ui/widgets/widget")

--- Menu widget.
-- @type UiMenu
local UiMenu = Class("UiMenu", UiWidget)

--- Creates a new selector widget.
-- @param clss UiMenu class.
-- @param widgets List of widgets. Nil for an empty menu.
-- @return UiMenu.
UiMenu.new = function(clss, widgets)
	local self = Widgets.Uiwidget.new(clss)
	self.hint = "$A: Select\n$$B\n$$U\n$$D"
	-- Create the popup menu.
	self.__box = UiVBox()
	if widgets then
		for k,v in ipairs(widgets) do
			self.__box:add_child(v)
		end
	end
	-- Create the input handler.
	self.__input = InputHandler(Client.bindings, self.__box)
	self.__input.focus_changed = function()
		Ui:update_help()
		Ui:autoscroll()
	end
	self.__input.handle_back = function()
		self:set_menu_opened(false)
	end
	self.__input.handle_left = self.__input.handle_back
	self.__input.handle_menu = self.__input.handle_back
	self.__input:set_enabled(false)
	return self
end

--- Adds a widget to the menu.
-- @param self UiMenu.
-- @param widget Widget.
UiMenu.add_widget = function(self, widget)
	self.__box:add_child(widget)
end

--- Handles the apply command.
-- @param self UiMenu.
UiMenu.apply = function(self)
	self:set_menu_opened(true)
end

--- Clears the widgets of the menu.
-- @param self UiMenu.
UiMenu.clear = function(self)
	self.__box:clear()
end

--- Handles an event.
-- @param self UiMenu.
-- @param args Event.
-- @return True to bubble the event. False otherwise.
UiMenu.handle_event = function(self, args)
	if not self.__opened then
		return UiWidget.handle_event(self, args)
	end
	return self.__input:handle_event(args)
end

--- Updates the widget.
-- @param self UiMenu.
-- @param secs Seconds since the last update.
UiMenu.update = function(self, secs)
	UiWidget.update(self, secs)
	self.__input:update(secs)
end

--- Sets the focus state of the widget.
-- @param self UiMenu.
-- @param value True to focus. False otherwise.
-- @return True if the focus changed. False if the widget rejected the change.
UiMenu.set_focused = function(self, value)
	if self.focused == value then return true end
	if self.__opened and not value then return end
	return UiWidget.set_focused(self, value)
end

--- Sets the popup state of the menu.
-- @param self UiMenu.
-- @param value True to popup. False otherwise.
UiMenu.set_menu_opened = function(self, value)
	if self.__opened == value then return end
	if value then
		-- Add the box to the UI.
		local x = self:get_x() + self:get_width() + 5
		local y = -self:get_y()
		Ui:add_temporary(x, y, self.__box)
		self.__box:focus_index(1)
		-- Grab input.
		self.__opened = true
		self.__input:set_enabled(true)
		Ui.input:set_enabled(false)
		-- Play the effect.
		Client.effects:play_global("uitransition1")
	else
		-- Remove the box from the UI.
		Ui:remove_temporary(self.__box)
		-- Ungrab the input.
		self.__opened = nil
		self.__input:set_enabled(false)
		Ui.input:set_enabled(true)
		-- Play the effect.
		Client.effects:play_global("uitransition1")
	end
end

--- Sets the widgets of the menu.
-- @param self UiMenu.
-- @param value List of widgets.
UiMenu.set_menu_widgets = function(self, value)
	self.__box:clear()
	for k,v in ipairs(value) do
		self.__box:add_child(v)
	end
end

return UiMenu
