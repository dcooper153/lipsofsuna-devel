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
local Graphics = require("system/graphics")
local UiLabel = require("ui/widgets/label")
local UiMenu = require("ui/widgets/menu")

--- Selector widget.
-- @type UiSelector
local UiSelector = Class("UiSelector", UiMenu)

--- Creates a new selector widget.
-- @param clss UiSelector class.
-- @param label Label of the widget.
-- @param choices List of choices.
-- @return UiSelector.
UiSelector.new = function(clss, label, choices)
	local self = UiMenu.new(clss)
	self.hint = "$A: Select\n$$B\n$$U\n$$D"
	self.label = label
	self.text = ""
	self.choice = nil
	self.choices = choices
	-- Create the menu widgets.
	local widgets = {}
	for k,v in ipairs(self.choices) do
		local index = k
		local widget = UiLabel(v[1])
		local func = v[2]
		widget.apply = function()
			func()
			self:select(index)
			self:set_menu_opened(false)
		end
		table.insert(widgets, widget)
	end
	self:set_menu_widgets(widgets)
	return self
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
	local size = UiMenu.rebuild_size(self)
	-- Resize to fit the label.
	if self.text then
		local w,h = Graphics:measure_text(Theme.text_font_1, self.text, size.x - 10)
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

UiSelector.rebuild_canvas = function(self)
	-- Add the base.
	local w = self.size.x - Theme.width_label_1 - 5
	local w1 = self.size.x - Theme.width_label_1 - 15
	local h = self.size.y - 10
	UiMenu.rebuild_canvas(self)
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

return UiSelector
