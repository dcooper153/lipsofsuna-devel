require "client/widgets/listwidget"

Widgets.Skills = Class(Widgets.List)

Widgets.Skills.getter = function(self, key, value)
	if key == "total" then
		local t = 0
		for k,v in pairs(self.dict_id) do t = t + v.value end
		return t
	else
		return Widget.getter(self, key, value)
	end
end

--- Creates a new skills widget.
-- @param clss Skills widget class.
-- @return Skills widget.
Widgets.Skills.new = function(clss)
	local self = Widget.new(clss, {cols = 1})
	self.dict_id = {}
	self.dict_row = {}
	self:add("dexterity", "Dexterity", "Determines the effectiveness of your ranged attacks, as well as how fast you can move and react.")
	self:add("health", "Health", "Determines how much damage your can withstand.")
	self:add("intelligence", "Intelligence", "Determines what items you can craft and how effectively you can handle technology based items.")
	self:add("perception", "Perception", "Determines how far you can see and how well you can handle precision weapons and items.")
	self:add("strength", "Strength", "Determines your physical power and the effectiveness of your melee attacks.")
	self:add("willpower", "Willpower", "Determines what spells you can cast and how effective they are.")
	return self
end

--- Adds a skill.
-- @param self Skills widget.
-- @param id Skill id.
-- @param name Skill name.
-- @param desc Skill description.
Widgets.Skills.add = function(self, id, name, desc)
	local index = #self.dict_row + 1
	local skill = Widgets.SkillControl{
		cap = 0, desc = desc, id = id, index = index,
		max = 100, name = name, text = name, value = 0,
		pressed = function(w)
			local v = w:get_value_at(Client.cursor_pos)
			if v and self.species then v = math.min(v, self.species.skill_quota + w.value - self.total) end
			if v then
				w.cap = v
				self:changed(w)
			end
			self:shown(w)
		end}
	self.dict_id[id] = skill
	self.dict_row[index] = skill
	self:append_row(skill)
end

--- Called when skill has changed.
-- @param self Skills widget.
-- @param widget Skill control widget.
Widgets.Skills.changed = function(self, widget)
end

--- Gets the value of a skill.
-- @param self Skills widget.
-- @param id Skill identifier string.
-- @return Skill value.
Widgets.Skills.get_value = function(self, id)
	return self.dict_id[id].value
end

--- Sets the species for which the skills are.
-- @param self Skills widget.
-- @param value Species.
Widgets.Skills.set_species = function(self, value)
	self.species = value
	for k,v in pairs(value.skills) do
		local w = self.dict_id[k]
		if w then
			w.max = v.max
			w.cap = v.val
			w.value = v.val
		end
	end
end

--- Shows a skill.
-- @param self Skills widget.
-- @param index Skill index.
Widgets.Skills.show = function(self, index)
	self:shown(self.dict_row[index])
end

--- Called when a skill is shown.
-- @param self Skills widget.
-- @param index Skill number.
Widgets.Skills.shown = function(self, widget)
end
