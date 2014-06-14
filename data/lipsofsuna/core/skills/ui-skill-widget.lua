--- Skill configuration widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.skills.skill_widget
-- @alias UiSkillWidget

local Class = require("system/class")
local UiMenu = require("ui/widgets/menu")
local UiWidget = require("ui/widgets/widget")

--- Skill configuration widget.
-- @type UiSkillWidget
local UiSkillWidget = Class("UiSkillWidget", UiMenu)

--- Creates a new skill widget.
-- @param clss UiSkillWidget class.
-- @param skill SkillSpec.
-- @param active True if the requirements are met. False otherwise.
-- @param value True if enabled by the user. False otherwise.
-- @return UiSkillWidget.
UiSkillWidget.new = function(clss, skill, active, value)
	local self = UiMenu.new(clss)
	self.skill = skill
	self.active = active
	self.value = value
	self.hint = (value and "$A: Disable\n$$B\n$$U\n$$D") or
	            (active and "$A: Enable\n$$B\n$$U\n$$D") or "$$B\n$$U\n$$D"
	return self
end

UiSkillWidget.apply = function(self)
	if not self.active then return end
	-- Direction popup for combat arts.
	if not self.value and self.skill.combat then
		self:clear()
		self:add_button("Stand", function(w) self:enable_skill("stand") end)
		self:add_button("Front", function(w) self:enable_skill("front") end)
		self:add_button("Back", function(w) self:enable_skill("back") end)
		self:add_button("Left", function(w) self:enable_skill("left") end)
		self:add_button("Right", function(w) self:enable_skill("right") end)
		return UiMenu.apply(self)
	end
	-- Toggle the value.
	if self.value then
		self:disable_skill()
	else
		self:enable_skill("")
	end
end

UiSkillWidget.disable_skill = function(self)
	-- Toggle the value.
	self.value = false
	self.hint = "$A: Enable\n$$B\n$$U\n$$D"
	self.need_repaint = true
	-- Remove the skill.
	Main.client_skills:remove(self.skill.name)
	Ui:restart_state()
	Main.effect_manager:play_global("uitoggle2")
	-- Send an update.
	Main.messaging:client_event("update skills", Main.client_skills:get_names())
end

UiSkillWidget.enable_skill = function(self, dir)
	-- Toggle the value.
	self.value = true
	self.hint = "$A: Disable\n$$B\n$$U\n$$D"
	self.need_repaint = true
	-- Add the skill.
	Main.client_skills:add(self.skill.name, dir)
	Main.effect_manager:play_global("uitoggle1")
	-- Send an update.
	Main.messaging:client_event("update skills", Main.client_skills:get_names())
end

UiSkillWidget.rebuild_size = function(self)
	-- Get the base size.
	local size = UiWidget.rebuild_size(self)
	-- Resize to fit the description.
	if self.skill then
		local w1,h1 = Program:measure_text(Theme.text_font_2, self.skill.name, size.x-5-Theme.width_icon_1)
		local w2,h2 = Program:measure_text(Theme.text_font_1, self.skill.description, size.x-5-Theme.width_icon_1)
		local h = (h1 or 0) + (h2 or 0)
		size.y = math.max(size.y, h + 10)
		size.y = math.max(size.y, Theme.width_icon_1+10)
		self.title_height = h1 or 0
	end
	return size
end

UiSkillWidget.rebuild_canvas = function(self)
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	UiWidget.rebuild_canvas(self)
	-- Add the icon.
	if self.skill then
		Theme:draw_icon_scaled(self, self.skill.icon or "missing1",
			5, 5, Theme.width_icon_1, Theme.width_icon_1,
			self.value and {1,1,1,1} or {0.3,0.3,0.3,0.3})
	end
	-- Add the name.
	if self.skill then
		self:canvas_text{
			dest_position = {5+Theme.width_icon_1,5},
			dest_size = {w-5-Theme.width_icon_1,h},
			text = self.skill.name,
			text_alignment = {0,0},
			text_color = self.active and Theme.text_color_1 or Theme.text_color_2,
			text_font = Theme.text_font_2}
	end
	-- Add the description.
	if self.skill and self.title_height then
		self:canvas_text{
			dest_position = {5+Theme.width_icon_1,5+self.title_height},
			dest_size = {w-5-Theme.width_icon_1,h},
			text = self.skill.description,
			text_alignment = {0,0},
			text_color = self.active and Theme.text_color_1 or Theme.text_color_2,
			text_font = Theme.text_font_1}
	end
end

return UiSkillWidget
