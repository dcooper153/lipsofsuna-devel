--- Choice widget for NPC dialogs.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.widgets.dialog_choice
-- @alias UiDialogChoice

local Class = require("system/class")
local UiLabel = require("ui/widgets/label")

--- Choice widget for NPC dialogs.
-- @type UiDialogChoice
local UiDialogChoice = Class("UiDialogChoice", UiLabel)

UiDialogChoice.new = function(clss, id, index, text)
	local self = UiLabel.new(clss, text)
	self.dialog_id = id
	self.dialog_index = index
	self.hint = "$A: Reply\n$$B\n$$U\n$$D"
	return self
end

UiDialogChoice.apply = function(self)
	Main.messaging:client_event("choose dialog answer", self.dialog_id, self.text)
	Client.effects:play_global("transition1")
end

UiDialogChoice.rebuild_canvas = function(self)
	-- Add the background.
	Theme:draw_button(self, nil,
		0, 0, self.size.x, self.size.y,
		self.focused, false)
	-- Add the text.
	self:canvas_text{
		dest_position = {5,5},
		dest_size = {self.size.x-10,self.size.y-10},
		text = self.text,
		text_alignment = {0,0},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

return UiDialogChoice
