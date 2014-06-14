--- Spell widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.widgets.spell
-- @alias UiSpell

local Class = require("system/class")
local UiWidget = require("ui/widgets/widget")

--- Spell widget.
-- @type UiSpell
local UiSpell = Class("UiSpell", UiWidget)

--- Creates a new spell widget.
-- @param spec Action or modifier spec.
-- @param active True for active. False for grayed out.
-- @return UiSpell
UiSpell.new = function(clss, spec, active)
	local self = UiWidget.new(clss)
	self.spec = spec
	self.mode = spec and spec.type or "modifier"
	self.active = active
	self.hint = active and "$A: Select\n$$B\n$$U\n$$D" or "$$B\n$$U\n$$D"
	self.icon = self.spec and Main.specs:find_by_name("IconSpec", self.spec.icon or "missing")
	return self
end

--- Recalculates the size of the widget.
-- @param self UiSpell.
UiSpell.rebuild_size = function(self)
	-- Get the base size.
	local size = UiWidget.rebuild_size(self)
	-- Resize to fit the description.
	if self.spec then
		local w1,h1 = Program:measure_text(Theme.text_font_2, self:get_pretty_name(), size.x-5-Theme.width_icon_1)
		local w2,h2 = Program:measure_text(Theme.text_font_1, self.spec.description, size.x-5-Theme.width_icon_1)
		local h = (h1 or 0) + (h2 or 0)
		size.y = math.max(size.y, h + 10)
		size.y = math.max(size.y, Theme.width_icon_1+10)
		self.title_height = h1 or 0
	end
	return size
end

--- Redraws the canvas.
-- @param self UiSpell.
UiSpell.rebuild_canvas = function(self)
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	UiWidget.rebuild_canvas(self)
	-- Add the icon.
	if self.spec then
		Theme:draw_icon_scaled(self, self.spec.icon,
			5, 5, Theme.width_icon_1, Theme.width_icon_1,
			self.active and {1,1,1,1} or {0.3,0.3,0.3,0.3})
	end
	-- Add the name.
	if self.spec then
		local cap = self:get_pretty_name()
		self:canvas_text{
			dest_position = {5+Theme.width_icon_1,5},
			dest_size = {w-5-Theme.width_icon_1,h},
			text = cap,
			text_alignment = {0,0},
			text_color = self.active and Theme.text_color_1 or Theme.text_color_2,
			text_font = Theme.text_font_2}
	end
	-- Add the description.
	if self.spec and self.title_height then
		self:canvas_text{
			dest_position = {5+Theme.width_icon_1,5+self.title_height},
			dest_size = {w-5-Theme.width_icon_1,h},
			text = self.spec.description,
			text_alignment = {0,0},
			text_color = self.active and Theme.text_color_1 or Theme.text_color_2,
			text_font = Theme.text_font_1}
	end
end

--- Gets the human readable name of the displayed spell.
-- @param self UiSpell.
UiSpell.get_pretty_name = function(self)
	if not self.spec then return end
	return string.gsub(self.spec.name, "(.)(.*)", function(a,b) return string.upper(a) .. b end)
end

return UiSpell
