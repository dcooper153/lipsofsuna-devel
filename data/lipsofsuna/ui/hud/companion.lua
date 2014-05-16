--- Companion HUD widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.hud.companion
-- @alias HudCompanion

local Class = require("system/class")
local Widget = require("system/widget")

--- Companion HUD widget.
-- @type HudCompanion
local HudCompanion = Class("HudCompanion", Widget)

HudCompanion.new = function(clss)
	local self = Widget.new(clss)
	self:set_offset(Vector())
	return self
end

HudCompanion.apply = function(self)
	local dialog,companion = self:get_dialog()
	if not dialog then return end
	if dialog.type == "choice" then
		-- TODO
		--Main.messaging:client_event("choose dialog answer", companion.id, text)
		--Client.effects:play_global("transition1")
	else
		Main.messaging:client_event("choose dialog answer", companion:get_id(), "")
		Client.effects:play_global("uitransition1")
	end
end

HudCompanion.reshaped = function(self)
	local w = Theme.text_height_1*20
	self:calculate_request{
		font = Theme.text_font_1,
		internal = true,
		text = self.__text or "",
		paddings = {7,7,64,7},
		width = w-14,
		height = Theme.text_height_1*2}
	self:canvas_clear()
	if self.__text then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,self:get_height()},
			source_image = "widgets3",
			source_position = {640,80},
			source_tiling = {-1,320,-1,-1,80,-1}}
		self:canvas_text{
			dest_position = {64,7},
			dest_size = {w-64-7,self:get_height()-10},
			text = self.__text,
			text_alignment = {0,0},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_3}
	end
end

HudCompanion.update = function(self, secs)
	local dialog = self:get_dialog()
	local text = dialog and dialog.message or nil
	if text ~= self.__text then
		self.__text = text
		self:reshaped()
	end
end

--- Gets the dialog of player's companion.
-- @param self HudCompanion.
-- @return Dialog if available. Nil otherwise.
HudCompanion.get_dialog = function(self)
	-- FIXME: Won't work for multiplayer.
	local player = Client.player_object
	if not player then return end
	local companion = player.companion
	if not companion then return end
	return companion.dialog,companion
end

HudCompanion.get_text = function(self)
	return self.__text
end

HudCompanion.set_text = function(self, v)
	if self.__text == v then return end
	self.__text = v
	self.timer = 0
	self:reshaped()
	self:set_visible(true)
end

return HudCompanion
