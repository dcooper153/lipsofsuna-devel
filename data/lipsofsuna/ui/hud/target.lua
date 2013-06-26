--- HUD target action display widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.hud.target
-- @alias Hudtarget

local Class = require("system/class")
local Widget = require("system/widget")

--- HUD target action display widget.
-- @type Hudtarget
local Hudtarget = Class("Hudtarget", Widget)

Hudtarget.new = function(clss)
	local self = Widget.new(clss)
	self.update_timer = 0
	return self
end

Hudtarget.reshaped = function(self)
	local w = self:get_width()
	local h = self:get_height()
	self:canvas_clear()
	Theme:draw_scene_text(self, self.text, 0, 0, w, h, 1)
end

Hudtarget.update = function(self, secs)
	-- Update periodically.
	self.update_timer = self.update_timer + secs
	if self.update_timer < 0.1 then return end
	self.update_timer = 0
	-- Hide if no targetable object
	local obj = Operators.world:get_target_object()
	if not obj or not Operators.world:get_target_usages() then
		self:set_visible(false)
		return
	end
	-- Format the text.
	local key = Client.bindings:get_control_name("menu_apply") or "[---]"
	if obj:get_count() > 1 then
		self.text = string.format("%s %s (%d)", key, obj.spec.name, obj:get_count())
	else
		self.text = string.format("%s %s", key, obj.spec.name)
	end
	-- Update the position.
	local mode = Program:get_video_mode()
	local padx = mode[1] - self:get_width()
	self:set_offset(Vector(padx / 2, Theme.width_icon_1 + Theme.text_height_1 * 2.5))
	-- Show the widget.
	self:reshaped()
	self:set_visible(true)
end

return Hudtarget
