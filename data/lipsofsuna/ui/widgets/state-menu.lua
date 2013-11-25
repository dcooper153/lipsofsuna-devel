--- Radio menu widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.widgets.radio_menu
-- @alias UiStateMenu

local Class = require("system/class")
local UiMenu = require("ui/widgets/menu")

--- Radio menu widget.
-- @type UiStateMenu
local UiStateMenu = Class("UiStateMenu", UiMenu)

--- Creates a new UI state menu.
-- @param clss UiStateMenu class.
-- @param label Label.
-- @param state State name.
-- @return UiStateMenu.
UiStateMenu.new = function(clss, label, state)
	local self = UiMenu.new(clss)
	self.label = label
	self.state = state
	return self
end

--- Handles the apply command.
-- @param self UiMenu.
UiStateMenu.apply = function(self)
	-- Remove old widgets.
	self:clear()
	-- Find the state.
	if not self.state then return end
	local state_ = Ui.states[self.state]
	if not state_ then return end
	-- Call the initializers.
	for id,func in pairs(state_.init) do
		local ret,widgets = xpcall(func, function(err)
			print(debug.traceback("ERROR: " .. err))
		end)
		if ret and widgets then
			for k,widget in ipairs(widgets) do self:add_widget(widget) end
		end
	end
	-- Create the widgets.
	for id,spec in ipairs(state_.widgets) do
		local ret,widget = xpcall(spec.init, function(err)
			print(debug.traceback("ERROR: " .. err))
		end)
		if ret and widget then
			if widget.class_name then
				if spec.help then widget:set_help(spec.help) end
				if spec.hint then widget:set_hint(spec.hint) end
				if spec.input then widget.handle_input = spec.input end
				if spec.update then widget.update = spec.update end
				if not widget.id then widget.id = spec.id or id end
				self:add_widget(widget)
			else
				for k,v in ipairs(widget) do self:add_widget(v) end
			end
		end
	end
	-- Show the menu.
	UiMenu.apply(self)
end

--- Rebuilds the canvas.
-- @param self UiStateMenu.
UiStateMenu.rebuild_canvas = function(self)
	Theme:draw_button(self, self.label,
		0, 0, self.size.x, self.size.y, self.focused, false)
end

return UiStateMenu
