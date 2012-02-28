Views.Skills = Class(Widget)

--- Creates a new skills widget.
-- @param clss skills class.
-- @return Skills widget.
Views.Skills.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 2, spacings = {0,0}})
	self.skills = Widgets.Skills()
	return self
end

Views.Skills.back = function(self)
end

Views.Skills.enter = function(self, from, level)
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
		self.skills:update_points()
	end
end
