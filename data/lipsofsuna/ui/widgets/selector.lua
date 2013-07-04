--- Selector widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.widgets.selector
-- @alias UiSelector

local Class = require("system/class")
local Input = require("system/input")
local InputHandler = require("ui/input-handler")
local Keysym = require("system/keysym")
local UiLabel = require("ui/widgets/label")
local UiVBox = require("ui/widgets/vbox")
local UiWidget = require("ui/widgets/widget")

--- Selector widget.
-- @type UiVBox
local UiSelector = Class("UiSelector", UiWidget)

--- Creates a new selector widget.
-- @param clss UiSelector class.
-- @param label Label of the widget.
-- @param choices List of choices.
-- @return UiSelector.
UiSelector.new = function(clss, label, choices)
	local self = Widgets.Uiwidget.new(clss)
	self.hint = "$A: Select\n$$B\n$$U\n$$D"
	self.label = label
	self.text = ""
	self.choice = nil
	self.choices = choices
	-- Create the popup menu.
	self.box = UiVBox()
	for k,v in ipairs(self.choices) do
		local index = k
		local widget = UiLabel(v[1])
		local func = v[2]
		widget.apply = function()
			func()
			self:select(index)
			self:stop_grabbing()
		end
		self.box:add_child(widget)
	end
	-- Create the input handler.
	self.input = InputHandler(Client.bindings, self.box)
	self.input.focus_changed = function()
		Ui:update_help()
		Ui:autoscroll()
	end
	self.input.handle_back = function()
		self:stop_grabbing()
	end
	self.input.handle_left = self.input.handle_back
	self.input.handle_menu = self.input.handle_back
	self.input:set_enabled(false)
	return self
end

UiSelector.apply = function(self)
	-- Add the box to the UI.
	local x = self:get_x() + self:get_width() + 5
	local y = -self:get_y()
	Ui:add_temporary(x, y, self.box)
	-- Grab input.
	self.selecting = true
	self.input:set_enabled(true)
	Ui.input:set_enabled(false)
	-- Play the effect.
	Client.effects:play_global("uitransition1")
end

UiSelector.handle_event = function(self, args)
	if not self.selecting then
		return UiWidget.handle_event(self, args)
	end
	self.input:handle_event(args)
end

UiSelector.stop_grabbing = function(self)
	self.selecting = nil
	self.input:set_enabled(false)
	Ui.input:set_enabled(true)
	Ui:remove_temporary(self.box)
	Client.effects:play_global("uitransition1")
end

UiSelector.update = function(self, secs)
	UiWidget.update(self, secs)
	self.input:update(secs)
end

--- Selects one of the options.
-- @param self UiSelector.
-- @param value Index of the choice.
UiSelector.select = function(self, value)
	self.choice = value
	self.text = value and self.choices[value] and self.choices[value][1] or ""
	self.need_repaint = true
end

UiSelector.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.text then
		local w,h = Program:measure_text(Theme.text_font_1, self.text, size.x - 10)
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

UiSelector.rebuild_canvas = function(self)
	-- Add the base.
	local w = self.size.x - Theme.width_label_1 - 5
	local w1 = self.size.x - Theme.width_label_1 - 15
	local h = self.size.y - 10
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the text.
	-- FIXME
	self:canvas_text{
		dest_position = {Theme.width_label_1+2,5+Theme.text_pad_1},
		dest_size = {w1,h},
		text = self.text,
		text_alignment = {0,0.5},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

--- Sets the focus state of the widget.
-- @param self UiSelector.
-- @param value True to focus. False otherwise.
-- @return True if the focus changed. False if the widget rejected the change.
UiSelector.set_focused = function(self, value)
	if self.focused == value then return true end
	if self.selecting and not value then return end
	return UiWidget.set_focused(self, value)
end

return UiSelector
