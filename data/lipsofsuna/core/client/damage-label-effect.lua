local Class = require("system/class")
local Label = require("system/widgets/label")
local Render = require("system/render")
local Vector = require("system/math/vector")

local enemy_colors = {[false] = {1,1,0,1}, [true] = {0,1,1,1}}
local player_colors = {[false] = {1,0,0,1}, [true] = {0,1,0,1}}

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
	self.life = 3
	self.fade = 1
	self.position = point or object:get_position() + Vector(0,2,0)
	-- Create the text widget.
	self.widget = Label()
	self.widget:set_text(string.format("%d", damage))
	self.widget:set_font("medium")
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
	self.life = self.life - secs
	if self.life < 0 then
		self:disable()
		return
	end
	-- Update the text position.
	local proj = self:get_screen_position()
	self.widget:set_offset(proj)
	-- Update the text color.
	local color = self.widget:get_color()
	if proj.z > 1 then
		color[4] = 0
	elseif self.life < self.fade then
		color[4] = self.life / self.fade
	else
		color[4] = 1
	end
	self.widget:set_color(color)
end

DamageLabelEffect.get_screen_position = function(self)
	-- TODO: Better movement.
	local offset = Vector(0, 3 - self.life, 0)
	local proj = Render:project(self.position + offset)
	proj.x = proj.x - 125
	return proj
end

return DamageLabelEffect
