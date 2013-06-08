--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.effect.speech_bubble_effect
-- @alias SpeechBubbleEffect

local Class = require("system/class")
local Label = require("system/widgets/label")
local Render = require("system/render")
local Vector = require("system/math/vector")

--- TODO:doc
-- @type SpeechBubbleEffect
local SpeechBubbleEffect = Class("SpeechBubbleEffect")

--- Creates a new text bubble.
-- @param clss SpeechBubbleEffect class.
-- @param args Arguments.<ul>
--   <li>font: Font name.</li>
--   <li>font_color: Font color table.</li>
--   <li>life: Life time in seconds.</li>
--   <li>node: Parent node or nil.</li>
--   <li>object: Parent object or nil.</li>
--   <li>position: Position in world space.</li>
--   <li>text: Text effect string.</li>
--   <li>velocity: Velocity vector.</li></ul>
-- @return SpeechBubbleEffect
SpeechBubbleEffect.new = function(clss, args)
	local self = Class.new(clss)
	for k,v in pairs(args) do self[k] = v end
	self.offset = self.offset or Vector()
	self.widgets = {}
	self:add_line(args)
	return self
end

SpeechBubbleEffect.add_line = function(self, args)
	-- Create the text widget.
	local life = args.life or 10
	local fade = args.fade or 0
	local widget = Label()
	widget:set_text(args.text)
	widget:set_color(args.text_color)
	widget:set_font(args.text_font)
	widget:set_request(250, nil)
	widget:set_halign(0.5)
	widget.life = life
	widget.fade = fade
	widget.velocity = args.velocity
	widget.motion = Vector()
	-- Add it to the widget list.
	table.insert(self.widgets, widget)
	self:transform()
	Ui:add_speech_bubble(widget)
end

SpeechBubbleEffect.disable = function(self)
	-- Remove from the effect manager.
	if self.object then
		Client.effects.speech_bubble_dict_id[self.object:get_id()] = nil
	end
	Client.effects.speech_bubble_dict[self] = nil
	-- Remove from the UI.
	for k,v in pairs(self.widgets) do
		Ui:remove_speech_bubble(v)
	end
	self.widgets = nil
end

SpeechBubbleEffect.transform = function(self)
	if not self.object then return end
	local p
	if self.node and self.object.render then
		p = self.object.render:find_node{name = self.node}
	end
	if p then
		self.position = self.offset + self.object:get_position() + self.object:get_rotation() * p
	else
		self.position = self.offset + self.object:get_position()
	end
end

SpeechBubbleEffect.update = function(self, secs)
	-- Remove if the object disappeared.
	if self.object and not self.object:get_visible() then
		self:disable()
		return
	end
	-- Remove expired text widgets.
	local n = #self.widgets
	for i = 1,n do
		local v = self.widgets[n - i + 1]
		v.life = v.life - secs
		if v.life < 0 then
			Ui:remove_speech_bubble(v)
			table.remove(self.widgets, n - i + 1)
		end
	end
	-- Remove if no more text widgets left.
	n = #self.widgets
	if n == 0 then
		self:disable()
		return
	end
	-- Update text motion.
	for i = 1,n do
		local v = self.widgets[n - i + 1]
		if v.velocity then
			v.motion = v.motion + v.velocity * secs
		end
	end
	-- Calculate the position.
	self:transform()
	local p = Render:project(self.position)
	p.x = p.x - 125
	-- Move the text widgets.
	for i = 1,n do
		local v = self.widgets[n - i + 1]
		if v.life < v.fade then
			local c = v:get_color()
			v:set_color{c[1], c[2], c[3], v.life / v.fade}
		end
		if p.z > 1 then
			local c = v:get_color()
			v:set_color{c[1], c[2], c[3], 0}
		end
		v:set_offset(p + v.motion)
		if not v.velocity then
			p = Vector(p.x, p.y - v:get_height() - 5)
		end
	end
end

return SpeechBubbleEffect


