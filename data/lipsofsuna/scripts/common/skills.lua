---
-- Create and synchronize skills.
-- @name Skills
-- @class table
Skills = Class()
Skills.dict = {}
setmetatable(Skills.dict, {__mode = "k"})

--- Check if the object has enough skill.
-- @param clss Skills class.
-- @param args Arguments.<ul>
--   <li>owner: Object. (required)</li>
--   <li>skill: Skill name.</li>
--   <li>min: Minimum required value.</li></ul>
-- @return True if the user had enough skill.
Skills.check = function(clss, args)
	if not args.skill or not args.min then return true end
	if not args.owner then return end
	local s = clss.dict[args.owner]
	local v = s and s.skills[args.skill]
	if not v or v.value < args.min then return end
	return true
end

--- Unregisters all skills.
-- @param self Skills.
Skills.clear = function(self)
	self.skills = {}
	Vision:event{type = "skills-cleared", object = self.owner}
end

--- Finds the skills for an object.
-- @param clss Skills class.
-- @param args Arguments.<ul>
--   <li>owner: Object.</li></ul>
-- @return Skills or nil.
Skills.find = function(clss, args)
	if not args.owner then return end
	return args.owner and clss.dict[args.owner]
end

--- Creates a new skills list.
-- @param clss Skills class.
-- @param args Arguments.
-- @return New skills.
Skills.new = function(clss, args)
	local s = Class.new(clss, args)
	s.skills = {}
	s.enabled = not args or args.enabled ~= false
	if s.owner then
		if clss.dict[s.owner] then error("object already has skills") end
		clss.dict[s.owner] = s
	end
	return s
end

--- Tries to subtract a value from the specified skill.
-- @param clss Skills class.
-- @param args Arguments.<ul>
--   <li>owner: Object. (required)</li>
--   <li>skill: Skill name. (required)</li>
--   <li>value: Value to subtract. (required)</li></ul>
-- @return True if the user had enough skill.
Skills.subtract = function(clss, args)
	if not args.skill or not args.value or not args.owner then return end
	local s = clss.dict[args.owner]
	local v = s and s.skills[args.skill]
	if not v or v.value < args.value then return end
	v.value = v.value - args.value
	Vision:event{type = "skill-changed", object = args.owner, skill = args.skill}
	return true
end

--- Gets the maximum value of a skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li></ul>
-- @return Number or nil.
Skills.get_maximum = function(self, args)
	local v = args.skill and self.skills[args.skill]
	return (v and v.maximum) or nil
end

--- Gets a list of skill names.
-- @param self Skills.
-- @return List of skill names.
Skills.get_names = function(self)
	local i = 1
	local n = {}
	for k,v in pairs(self.skills) do
		n[i] = k
		i = i + 1
	end
	return n
end

--- Gets the protection type of the skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li></ul>
-- @return String or nil.
Skills.get_protect = function(self, args)
	local v = args.skill and self.skills[args.skill]
	return (v and v.protect) or nil
end

--- Gets the regenreation speed of a skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li></ul>
-- @return Number or nil.
Skills.get_regen = function(self, args)
	local v = args.skill and self.skills[args.skill]
	return (v and v.regen) or nil
end

--- Gets information on a skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li></ul>
-- @return Skill information or nil.
Skills.get_skill = function(self, args)
	local v = args.skill and self.skills[args.skill]
	if not v then return end
	return {maximum = v.maximum, prot = v.prot, regen = v.regen, value = v.value}
end

--- Checks if a skill is present.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li></ul>
-- @return Boolean.
Skills.has_skill = function(self, args)
	return args.skill and self.skills[args.skill] ~= nil
end

--- Gets the value of a skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li></ul>
-- @return Number or nil.
Skills.get_value = function(self, args)
	local v = args.skill and self.skills[args.skill]
	return (v and v.value) or nil
end

--- Checks if a skill is present.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li></ul>
-- @return Boolean.
Skills.has_skill = function(self, args)
	return args.skill and self.skills[args.skill] ~= nil
end

--- Registers a skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>maximum: Maximum value.</li>
--   <li>name: Skill name. (required)</li>
--   <li>prot: Protection type. ("public"/"private"/"internal")</li>
--   <li>regen: Regeneration rate.</li>
--   <li>value: Initial value.</li></ul>
Skills.register = function(self, args)
	if not args.name then return end
	if self.skills[args.name] then error("skill already exists") end
	local v =
	{
		maximum = args.maximum or 0,
		regen = args.regen or 0,
		prot = args.prot or "private",
		value = math.max(0, math.min(args.value, args.maximum or 0))
	}
	self.skills[args.name] = v
	Vision:event{type = "skill-changed", object = self.owner, skill = args.name}
	return v
end

--- Sets attributes of a skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li>
--   <li>maximum: Maximum value.</li>
--   <li>regen: Regeneration rate.</li>
--   <li>value: Current value.</li></ul>
Skills.set = function(self, args)
	if not args.skill then return end
	local v = self.skills[args.skill]
	if not v then return end
	local old = math.floor(v.value)
	if args.maximum then
		v.maximum = args.maximum
		v.value = math.min(v.value, v.maximum)
	end
	if args.value then
		v.value = math.max(0, math.min(args.value, v.maximum))
	end
	if args.regen then
		v.regen = args.regen
	end
	if math.floor(v.value) ~= old then
		Vision:event{type = "skill-changed", object = self.owner, skill = args.skill}
	end
end

--- Sets the maximum value of a skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li>
--   <li>value: Number.</li></ul>
Skills.set_maximum = function(self, args)
	if not args.skill then return end
	local v = self.skills[args.skill]
	if not v then return end
	v.maximum = args.value
	v.value = math.min(v.value, v.maximum)
	Vision:event{type = "skill-changed", object = self.owner, skill = args.skill}
end

--- Sets the regeneration speed of a skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li>
--   <li>value: Number.</li></ul>
Skills.set_regen = function(self, args)
	if not args.skill then return end
	local v = self.skills[args.skill]
	if not v then return end
	v.regen = args.value
end

--- Sets the value of a skill.
-- @param self Skills.
-- @param args Arguments.<ul>
--   <li>skill: Skill name.</li>
--   <li>value: Number.</li></ul>
Skills.set_value = function(self, args)
	if not args.skill then return end
	local v = self.skills[args.skill]
	if not v then return end
	local old = math.floor(v.value)
	v.value = math.max(0, math.min(args.value or 0, v.maximum))
	if math.floor(v.value) ~= old then
		Vision:event{type = "skill-changed", object = self.owner, skill = args.skill}
	end
end

--- Owner object.
-- @name Skills.owner
-- @class table

Timer{delay = 1, func = function(self, secs)
	for k,v in pairs(Skills.dict) do
		if v.enabled then
			for l,w in pairs(v.skills) do
				v:set_value{skill = l, value = w.value + w.regen * secs}
			end
		end
	end
end}
