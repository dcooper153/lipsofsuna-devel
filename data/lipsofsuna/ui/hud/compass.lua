--- HUD compass widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.hud.compass
-- @alias Hudcompass

local Class = require("system/class")
local Graphics = require("system/graphics")
local UiWindow = require("ui/windows/ui-window")

--- HUD compass widget.
-- @type Hudcompass
local Hudcompass = Class("Hudcompass", UiWindow)

--- Creates a new compass widget.
-- @param clss Compass class.
-- @return Compass widget.
Hudcompass.new = function(clss)
	local self = UiWindow.new(clss)
	self.timer = 0
	return self
end

--- Called when the screen size changes.
-- @param self Hudcompass.
-- @param width Screen width.
-- @param height Screen height.
Hudcompass.screen_resized = function(self, w, h)
	self:set_position(0, h - 80)
end

--- Updates the compass.
-- @param self Compass widget.
-- @param secs Seconds since the last update.
Hudcompass.update = function(self, secs)
	self:set_visible(Ui.root == "play")
	self.timer = self.timer + secs
	if self.timer < 0.03 then return end
	self.timer = 0
	self.look_direction = math.pi - Client.player_state:get_turn_angle()
	self.quest_direction = Operators.quests:get_compass_direction()
	self.quest_distance = Operators.quests:get_compass_distance()
	self.quest_height = Operators.quests:get_compass_height()
	self:reshaped()
end

Hudcompass.reshaped = function(self)
	self:canvas_clear()
	-- Add the background.
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {74,74},
		rotation = self.look_direction,
		rotation_center = Vector(38,37),
		source_image = "compass1",
		source_position = {42,2},
		source_tiling = {0,74,0,0,74,0}}
	-- Add the quest marker.
	if self.quest_direction then
		local frac = 0.6 + 0.4 * math.min(1,self.quest_distance/50)
		self:canvas_image{
			dest_position = {32,44-33*frac},
			dest_size = {12,50},
			rotation = self.quest_direction + self.look_direction + math.pi,
			rotation_center = Vector(7,25),
			source_image = "compass1",
			source_position = {0,0},
			source_tiling = {0,12,0,0,50,0}}
	end
	-- Add the quest height offset.
	if self.quest_height then
		self:canvas_text{
			dest_position = {0,0},
			dest_size = {74,74},
			text = string.format("%+d", self.quest_height),
			text_alignment = {0.5,0.5},
			text_color = Theme.text_color_3,
			text_font = "tiny"}
	end
end

return Hudcompass
