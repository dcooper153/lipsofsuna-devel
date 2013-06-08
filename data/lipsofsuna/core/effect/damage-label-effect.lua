--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.effect.damage_label_effect
-- @alias DamageLabelEffect

local Class = require("system/class")
local Interpolation = require("system/math/interpolation")
local Label = require("system/widgets/label")
local Render = require("system/render")
local Vector = require("system/math/vector")
local Theme = require("ui/theme")

local enemy_colors = {[false] = {1,1,0,1}, [true] = {0,1,1,1}}
local player_colors = {[false] = {1,0,0,1}, [true] = {0,1,0,1}}
local text_path_a = {1,1,0.5,0,0}
local text_path_y = {0,0.5,0,-0.5,-1.5}

--- TODO:doc
-- @type DamageLabelEffect
local DamageLabelEffect = Class("DamageLabelEffect")

--- Creates a new damage text label.
-- @param clss DamageLabelEffect class.
-- @param object Damaged object, or nil.
-- @param point Position in world space, or nil.
-- @param damage Damage amount.
-- @return DamageLabelEffect.
DamageLabelEffect.new = function(clss, object, point, damage)
	local self = Class.new(clss)
	self.timer = 0
	self.position = point or object:get_position() + Vector(0,2,0)
	-- Create the text widget.
	self.widget = Label()
	self.widget:set_text(string.format("%d", damage))
	self.widget:set_font(Theme.text_font_2)
	self.widget:set_request(250, nil)
	self.widget:set_halign(0.5)
	-- Determine the text color.
	local player = Client.player_object
	local colors = object == player and player_colors or enemy_colors
	self.widget:set_color(colors[damage > 0])
	-- Add the widget to the screen.
	self.widget:set_offset(self:get_screen_position())
	Ui:add_speech_bubble(self.widget)
	return self
end

DamageLabelEffect.disable = function(self)
	-- Remove from the effect manager.
	Client.effects.speech_bubble_dict[self] = nil
	-- Remove from the UI.
	Ui:remove_speech_bubble(self.widget)
	self.widget = nil
end

DamageLabelEffect.update = function(self, secs)
	-- Remove if the text faded out completely.
	self.timer = self.timer + 1.8 * secs
	if self.timer >= 4 then
		return self:disable()
	end
	-- Update the text position.
	local proj = self:get_screen_position()
	self.widget:set_offset(proj)
	-- Update the text color.
	local color = self.widget:get_color()
	if proj.z > 1 then
		color[4] = 0
	else
		local a = Interpolation:interpolate_samples_cubic_1d(self.timer, text_path_a)
		color[4] = a
	end
	self.widget:set_color(color)
end

DamageLabelEffect.get_screen_position = function(self)
	local anim = Interpolation:interpolate_samples_cubic_1d(self.timer, text_path_y)
	local proj = Render:project(self.position + Vector(0, anim, 0))
	proj.x = proj.x - 125
	return proj
end

return DamageLabelEffect


