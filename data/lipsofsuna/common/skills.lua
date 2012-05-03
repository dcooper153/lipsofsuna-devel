--- Create and synchronize stats.
-- @name Stats
-- @class table
Skills = Class()
Skills.class_name = "Skills"

--- Creates a new skills list.
-- @param clss Skills class.
-- @param args Arguments.
-- @return New skills.
Skills.new = function(clss, args)
	local self = Class.new(clss, args)
	self.skills = {}
	return self
end

--- Adds a skill and all its requirements.
-- @param self Skills.
-- @param name Skill name.
Skills.add = function(self, name)
	-- Find the skill.
	local skill = Skillspec:find{name = name}
	if not skill then return end
	-- Add the skill.
	self.skills[name] = true
	-- Add the requirements.
	local reqs = skill:find_indirect_requirements()
	for name in pairs(reqs) do
		self.skills[name] = true
	end
end

--- Adds a skill without requirements.
-- @param self Skills.
-- @param name Skill name.
Skills.add_without_requirements = function(self, name)
	-- Find the skill.
	local skill = Skillspec:find{name = name}
	if not skill then return end
	-- Add the skill.
	self.skills[name] = true
end

--- Calculates actor attributes based on the skills.
-- @param self Skills.
-- @return Attribute table.
Skills.calculate_attributes = function(self)
	local attr = {
		explosives = 1,
		falling_damage = 1,
		guns = 1,
		jump = 1,
		max_health = 20,
		max_willpower = 20,
		melee = 1,
		ranged = 1,
		speed = 0.5,
		view_distance = 15}
	for k,v in pairs(self.skills) do
		local skill = Skillspec:find{name = k}
		if skill then skill.assign(attr) end
	end
	return attr
end

--- Calculates the damage multiplier for the given item.<br/>
--
-- The damage multiplier depends on the active skills and the type of the item.
-- The active skills determine the attributes of the actor. The attribute names
-- that affect the damage of the weapon are listed in the spec of the item.
--
-- @param self Skills.
-- @param item Item object.
-- @return Number.
Skills.calculate_damage_multiplier_for_item = function(self, item)
	if not item.spec.influences_bonus then return 1 end
	local mult = 1
	local attr = self:calculate_attributes()
	for name in pairs(item.spec.influences_bonus) do
		if attr[name] then mult = mult * attr[name] end
	end
	return mult
end

-- Calculates the damage multiplier for unarmed attacks.<br/>
--
-- The damage multiplier depends on the active skills only. More specifically,
-- the value of the "unarmed" attribute determines it.
--
-- @param self Skills.
-- @return Number.
Skills.calculate_damage_multiplier_for_unarmed = function(self, item)
	local attr = self:calculate_attributes()
	return attr["unarmed"] or 1
end

--- Removes all skills.
-- @param self Skills.
Skills.clear = function(self)
	self.skills = {}
end

--- Returns the dictionary of enabled skills.
-- @param self Skills.
-- @return Table of skills.
Skills.get_names = function(self)
	return self.skills
end

--- Gets the statuses of all skills, including those not enabled.<br/>
--
-- The status can be one of the following:<ul>
-- <li>"active": The skill is currently active.</li>
-- <li>"activable": The skill is not active, but its requirements are met.</li>
-- <li>"compatible": The skills are compatible with the actor.</li>
-- <li>"incompatible": The skills are incompatible with the actor.</li>
-- </ul>
--
-- @param self Skills.
-- @return Table of strings.
Skills.get_statuses = function(self)
	local skills = {}
	for name,skill in pairs(Skillspec.dict_name) do
		if self.skills[name] then
			skills[name] = "active"
		elseif self:is_activable(name) then
			skills[name] = "activable"
		elseif self:is_compatible(name) then
			skills[name] = "compatible"
		else
			skills[name] = "incompatible"
		end
	end
	return skills
end

--- Checks if the skill is active.
-- @param self Skills.
-- @param name Skill name.
-- @return True if the skill is active.
Skills.is_active = function(self, name)
	return self.skills[name] ~= nil
end

--- Checks if the requirements of the skill are met.
-- @param self Skills.
-- @param name Skill name.
-- @return True if the requirements are met.
Skills.is_activable = function(self, name)
	local skill = Skillspec:find{name = name}
	if not skill then return end
	for index,name in pairs(skill.requires) do
		if not self.skills[name] then return end
	end
	return true
end

--- Checks if the skill is compatible with the actor.
-- @param self Skills.
-- @param name Skill name.
-- @return True if the skill is compatible.
Skills.is_compatible = function(self, name)
	-- TODO
	return true
end

--- Removes a skill.
-- @param self Skills.
-- @param name Skill name.
Skills.remove = function(self, name)
	-- Find the skill.
	local skill = Skillspec:find{name = name}
	if not skill then return end
	-- Remove the skill.
	self.skills[name] = nil
	-- Remove the dependent skills.
	local deps = skill:find_indirectly_dependent()
	for name in pairs(deps) do
		self.skills[name] = nil
	end
end

--- Removes skills whose requirements are not met.
-- @param self Skills.
-- @param name Skill name.
Skills.remove_invalid = function(self)
	local more = true
	while more do
		more = false
		for name in pairs(self.skills) do
			local skill = Skillspec:find{name = name}
			if skill then
				-- Remove if the requirements are not met.
				local deps = skill:find_direct_requirements()
				for dep in pairs(deps) do
					if not self.skills[dep] then
						self.skills[name] = nil
						more = true
						break
					end
				end
			else
				-- Remove invalid skills.
				self.skills[name] = nil
				more = true
			end
		end
	end
end
