Views.Skills = Class(Widget)

--- Creates a new skills widget.
-- @param clss skills class.
-- @return Skills widget.
Views.Skills.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3, spacings = {0,0}})
	-- Create widgets.
	self.title = Widgets.Frame{style = "title", text = "Skills"}
	self.desc_text = Widgets.Text()
	self.skills = Widgets.Skills()
	self.skills.changed = function(widget, skill)
		Network:send{packet = Packet(packets.SKILLS, "string", skill.id, "float", skill.cap)}
	end
	self.skills.shown = function(widget, skill)
		self.desc_text.text = {{skill.name, "medium"}, {skill.desc}}
	end
	self.group = Widgets.Frame{cols = 1, rows = 1}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_child{col = 1, row = 1, widget = self.skills}
	self:set_child{col = 1, row = 1, widget = self.title}
	self:set_child{col = 1, row = 2, widget = self.group}
	self:set_child{col = 1, row = 3, widget = self.desc_text}
	self.skills:show(1)
	return self
end

Views.Skills.back = function(self)
	Gui:set_mode("menu")
end

--- Sets the species for which the skills are.
-- @param self Skills.
-- @param value Species.
Views.Skills.set_species = function(self, value)
	self.skills:set_species(value)
end

--- Updates a skill.
-- @param self Skills.
-- @param id Skill id.
-- @param value Current value.
-- @param cap Custom cap.
Views.Skills.update = function(self, id, value, cap)
	local skill = self.skills.dict_id[id]
	if skill then
		local species = Species:find{name = Player.species}
		local spec = species and species.skills[skill.id]
		skill.value = value
		skill.cap = cap
		skill.max = spec and spec.max or 100
		if id == "health" then
			Gui.skill_health.cap = cap
			Gui.skill_health.value = value
		elseif id == "willpower" then
			Gui.skill_mana.cap = cap
			Gui.skill_mana.value = value
		end
	end
end
