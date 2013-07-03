--- Standard user interface input handling.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.input_handler
-- @alias InputHandler

local Class = require("system/class")
local Input = require("system/input")

--- Standard user interface input handling.
-- @type InputHandler
local InputHandler = Class("InputHandler")

--- Creates a new input handler.
-- @param clss InputHandler class.
-- @param bindings Bindings.
-- @param box Box widget.
-- @return InputHandler.
InputHandler.new = function(clss, bindings, box)
	local self = Class.new(clss)
	self.__bindings = bindings
	self.__box = box
	self.__enabled = true
	return self
end

--- Called when the focused widget changes.
-- @param self InputHandler.
InputHandler.focus_changed = function(self)
end

--- Handles an input event.
-- @param self InputHandler.
-- @param args Event arguments.
InputHandler.handle_event = function(self, args)
	-- Copy the event just in case.
	local a = {}
	for k,v in pairs(args) do a[k] = v end
	-- Trigger standard action.
	-- TODO: Others.
	local action1 = self.__bindings:find_by_name("menu_up")
	local control1 = action1 and action1:get_event_response(a)
	if control1 ~= nil then
		self:handle_up(control1)
		return
	end
	local action2 = self.__bindings:find_by_name("menu_down")
	local control2 = action2 and action2:get_event_response(a)
	if control2 ~= nil then
		self:handle_down(control2)
		return
	end
	local action3 = self.__bindings:find_by_name("menu_left")
	local control3 = action3 and action3:get_event_response(a)
	if control3 ~= nil then
		self:handle_left(control3)
		return
	end
	local action4 = self.__bindings:find_by_name("menu_right")
	local control4 = action4 and action4:get_event_response(a)
	if control4 ~= nil then
		self:handle_right(control4)
		return
	end
	local action5 = self.__bindings:find_by_name("menu_apply")
	local control5 = action5 and action5:get_event_response(a)
	if control5 ~= nil then
		self:handle_apply(control5)
		return
	end
	local action6 = self.__bindings:find_by_name("menu_back")
	local control6 = action6 and action6:get_event_response(a)
	if control6 ~= nil then
		self:handle_back(control6)
		return
	end
	local action7 = self.__bindings:find_by_name("menu")
	local control7 = action7 and action7:get_event_response(a)
	if control7 ~= nil then
		self:handle_menu(control7)
		return
	end
	-- Trigger widget specific actions.
	if self.__box then
		local c = Input:get_pointer_position()
		local widget = self.__box:get_widget_by_point(c)
		if widget and widget.handle_event then
			if not widget:handle_event(args) then return end
		end
	end
end

--- Handles the apply event.
-- @param self InputHandler.
-- @param press True if pressed. False otherwise.
InputHandler.handle_apply = function(self, press)
	if press then
		local widget = self.__box:get_focused_widget()
		if widget and widget.apply then
			return widget:apply()
		end
	end
end

--- Handles the back event.
-- @param self InputHandler.
-- @param press True if pressed. False otherwise.
InputHandler.handle_back = function(self, press)
	if press then
		local widget = self.__box:get_focused_widget()
		if widget and widget.back then
			return widget:apply_back()
		end
	end
end

--- Handles the down browsing event.
-- @param self InputHandler.
-- @param press True if pressed. False otherwise.
InputHandler.handle_down = function(self, press)
	if press then
		self.__repeat_down = 0
		if self.__box and self.__box:focus_down() then
			self:focus_changed()
			Client.effects:play_global("uimove1")
		end
	else
		self.__repeat_down = nil
	end
end

--- Handles the left browsing event.
-- @param self InputHandler.
-- @param press True if pressed. False otherwise.
InputHandler.handle_left = function(self, press)
	if press then
		self.__repeat_left = 0
		if self.__box then
			local widget = self.__box:get_focused_widget()
			if widget and widget.left then widget:left() end
		end
	else
		self.__repeat_left = nil
	end
end

--- Handles the menu toggle event.
-- @param self InputHandler.
-- @param press True if pressed. False otherwise.
InputHandler.handle_menu = function(self, press)
end

--- Handles the right browsing event.
-- @param self InputHandler.
-- @param press True if pressed. False otherwise.
InputHandler.handle_right = function(self, press)
	if press then
		self.__repeat_right = 0
		if self.__box then
			local widget = self.__box:get_focused_widget()
			if widget and widget.right then widget:right() end
		end
	else
		self.__repeat_right = nil
	end
end

--- Handles the up browsing event.
-- @param self InputHandler.
-- @param press True if pressed. False otherwise.
InputHandler.handle_up = function(self, press)
	if press then
		self.__repeat_up = 0
		if self.__box and self.__box:focus_up() then
			self:focus_changed()
			Client.effects:play_global("uimove1")
		end
	else
		self.__repeat_up = nil
	end
end

--- Updates the input handler.
-- @param self InputHandler.
-- @param secs Seconds since the last update.
InputHandler.update = function(self, secs)
	-- Check if enabled.
	if not self.__enabled then
		self.__repeat_timer = 0
		return
	end
	-- Implement key repeat for browsing.
	local delay = 0.15
	if self.__repeat_up then
		self.__repeat_up = self.__repeat_up + secs
		if self.__repeat_up > delay then
			self.__repeat_up = 0
			local action = self.__bindings:find_by_name("menu_up")
			if action and action:is_pressed() then
				self:handle_up(true)
			end
		end
	end
	if self.__repeat_down then
		self.__repeat_down = self.__repeat_down + secs
		if self.__repeat_down > delay then
			self.__repeat_down = 0
			local action = self.__bindings:find_by_name("menu_down")
			if action and action:is_pressed() then
				self:handle_down(true)
			end
		end
	end
	if self.__repeat_left then
		self.__repeat_left = self.__repeat_left + secs
		if self.__repeat_left > delay then
			self.__repeat_left = 0
			local action = self.__bindings:find_by_name("menu_left")
			if action and action:is_pressed() then
				self:handle_left(true)
			end
		end
	end
	if self.__repeat_right then
		self.__repeat_right = self.__repeat_right + secs
		if self.__repeat_right > delay then
			self.__repeat_right = 0
			local action = self.__bindings:find_by_name("menu_right")
			if action and action:is_pressed() then
				self:handle_right(true)
			end
		end
	end
	-- Update mouse focus.
	if self.__box then
		local cursor = Input:get_pointer_position()
		local focus = self.__box:get_widget_by_point(cursor)
		if focus then
			local found,changed = self.__box:focus_widget(focus)
			if changed then
				self:focus_changed()
			end
		end
	end
end

--- Checks if the handler is enabled.
-- @param self InputHandler.
-- @param value True to enable. False to disable.
InputHandler.get_enabled = function(self)
	return self.__enabled
end

--- Toggles whether the handler is enabled.
-- @param self InputHandler.
-- @param value True to enable. False to disable.
InputHandler.set_enabled = function(self, value)
	self.__enabled = value
end

return InputHandler
