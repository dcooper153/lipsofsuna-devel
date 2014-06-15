--- Text widget for NPC dialogs.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.widgets.dialog_text
-- @alias UiDialogText

local Class = require("system/class")
local Graphics = require("system/graphics")
local UiWidget = require("ui/widgets/widget")

--- Text widget for NPC dialogs.
-- @type UiDialogText
local UiDialogText = Class("Uidialogtext", UiWidget)

UiDialogText.new = function(clss, id, index, char, text)
	local self = UiWidget.new(clss)
	self.dialog_id = id
	self.dialog_index = index
	self.char = char
	self.text = text
	self.hint = "$A: Next line\n$$B\n$$U\n$$D"
	return self
end

UiDialogText.apply = function(self)
	Main.messaging:client_event("choose dialog answer", self.dialog_id, "")
	Main.effect_manager:play_global("uitransition1")
end

UiDialogText.rebuild_size = function(self)
	-- Get the base size.
	local size = UiWidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.text and self.char then
		local w1,h1 = Graphics:measure_text(Theme.text_font_1, self.char, size.x - 25)
		local w2,h2 = Graphics:measure_text(Theme.text_font_1, self.text, size.x - 10)
		if h1 and h2 then size.y = math.max(size.y, h1 + h2 + 15) end
		self.title_width = w1
		self.title_height = math.max(20, (h1 or 0) + 5)
	end
	return size
end

UiDialogText.rebuild_canvas = function(self)
	if not self.title_height then return end
	-- Add the backgrounds.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,self.title_height},
		dest_size = {self.size.x, math.max(80, self.size.y-self.title_height)},
		source_image = "widgets3",
		source_position = {0,480},
		source_tiling = {-1,600,-1,-1,80,-1}}
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {self.size.x, self.title_height},
		source_image = "widgets3",
		source_position = {0,440},
		source_tiling = {-1,600,-1,-1,40,-1}}
	-- Add the character name.
	self:canvas_text{
		dest_position = {20,5},
		dest_size = {self.size.x-25,self.size.y-10},
		text = self.char,
		text_alignment = {0,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
	-- Add the text.
	self:canvas_text{
		dest_position = {5,self.title_height + 10},
		dest_size = {self.size.x-10,self.size.y-self.title_height-20},
		text = self.text,
		text_alignment = {0,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

return UiDialogText
