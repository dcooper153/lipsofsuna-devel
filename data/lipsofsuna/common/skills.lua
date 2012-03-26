--- Create and synchronize stats.
-- @name Stats
-- @class table
Stats = Class()

--- Creates a new stats list.
-- @param clss Stats class.
-- @param args Arguments.
-- @return New stats.
Stats.new = function(clss, args)
	local self = Class.new(clss, args)
	self.stats = {}
	self.enabled = not args or args.enabled ~= false
	self:register{
		prot = "public",
		name = "health",
		maximum = 20,
		value = 20,
		regen = 0.5}
	self:register{
		prot = "protected",
		name = "willpower",
		maximum = 20,
		value = 20,
		regen = 0.5}
	return self
end

--- Tries to subtract a value from the specified skill.
-- @param self Stats.
-- @param name Stat name.
-- @param value Value to subtract.
-- @return True if there was enough skill.
Stats.subtract = function(self, name, value)
	local v = self.stats[name]
	if not v or v.value < value then return end
	v.value = v.value - value
	Vision:event{type = "skill-changed", id = self.id, skill = name}
	return true
end

--- Gets the maximum value of a skill.
-- @param self Stats.
-- @param name Stat name.
-- @return Number, or nil.
Stats.get_maximum = function(self, name)
	local v = self.stats[name]
	return (v and v.maximum) or nil
end

--- Gets a list of skill names.
-- @param self Stats.
-- @return List of skill names.
Stats.get_names = function(self)
	local i = 1
	local n = {}
	for k,v in pairs(self.stats) do
		n[i] = k
		i = i + 1
	end
	return n
end

--- Gets the protection type of the skill.
-- @param self Stats.
-- @param name Stat name.
-- @return String, or nil.
Stats.get_protect = function(self, name)
	local v = self.stats[name]
	return (v and v.protect) or nil
end

--- Gets the regenreation speed of a skill.
-- @param self Stats.
-- @param name Stat name.
-- @return Number, or nil.
Stats.get_regen = function(self, name)
	local v = self.stats[name]
	return (v and v.regen) or nil
end

--- Gets information on a skill.
-- @param self Stats.
-- @param name Stat name.
-- @return Stat information, or nil.
Stats.get_skill = function(self, name)
	local v = self.stats[name]
	if not v then return end
	return {maximum = v.maximum, prot = v.prot, regen = v.regen, value = v.value}
end

--- Checks if a skill is present.
-- @param self Stats.
-- @param name Stat name.
-- @return Boolean.
Stats.has_skill = function(self, name)
	return self.stats[name] ~= nil
end

--- Gets the value of a skill.
-- @param self Stats.
-- @param name Stat name.
-- @return Number or nil.
Stats.get_value = function(self, name)
	local v = self.stats[name]
	return (v and v.value) or nil
end

--- Checks if a skill is present.
-- @param self Stats.
-- @param name Stat name.
-- @return Boolean.
Stats.has_skill = function(self, name)
	return self.stats[name] ~= nil
end

--- Registers a skill.
-- @param self Stats.
-- @param args Arguments.<ul>
--   <li>maximum: Maximum value.</li>
--   <li>name: Stat name. (required)</li>
--   <li>prot: Protection type. ("public"/"private"/"internal")</li>
--   <li>regen: Regeneration rate.</li>
--   <li>value: Initial value.</li></ul>
Stats.register = function(self, args)
	if not args.name then return end
	if self.stats[args.name] then error("skill already exists") end
	local v =
	{
		maximum = args.maximum or 0,
		regen = args.regen or 0,
		prot = args.prot or "private",
		value = math.max(0, math.min(args.value, args.maximum or 0))
	}
	self.stats[args.name] = v
	Vision:event{type = "skill-changed", id = self.id, skill = args.name}
	return v
end

--- Updates the stats.
-- @param self Stats.
-- @param secs Seconds since the last update.
Stats.update = function(self, secs)
	if not self.enabled then return end
	for k,v in pairs(self.stats) do
		self:set_value(k, v.value + v.regen * secs)
	end
end

--- Sets attributes of a skill.
-- @param self Stats.
-- @param name Stat name.
-- @param value Current value.
-- @param maximum Maximum value.
-- @param regen Regeneration rate.
Stats.set = function(self, name, value, maximum, regen)
	local v = self.stats[name]
	if not v then return end
	local old = math.floor(v.value)
	if maximum then
		v.maximum = maximum
		v.value = math.min(v.value, v.maximum)
	end
	if value then
		v.value = math.max(0, math.min(value, v.maximum))
	end
	if regen then
		v.regen = regen
	end
	if math.floor(v.value) ~= old then
		Vision:event{type = "skill-changed", id = self.id, skill = name}
	end
end

--- Sets the maximum value of a skill.
-- @param self Stats.
-- @param name Stat name.
-- @param value Number.
Stats.set_maximum = function(self, name, value)
	local v = self.stats[name]
	if not v then return end
	v.maximum = value
	v.value = math.min(v.value, v.maximum)
	Vision:event{type = "skill-changed", id = self.id, skill = name}
end

--- Sets the regeneration speed of a skill.
-- @param self Stats.
-- @param name Stat name.
-- @param value Number.
Stats.set_regen = function(self, name, value)
	local v = self.stats[name]
	if not v then return end
	v.regen = value
end

--- Sets the value of a skill.
-- @param self Stats.
-- @param name Stat name.
-- @param value Number.
Stats.set_value = function(self, name, value)
	local v = self.stats[name]
	if not v then return end
	local old = math.floor(v.value)
	v.value = math.max(0, math.min(value or 0, v.maximum))
	if math.floor(v.value) ~= old then
		Vision:event{type = "skill-changed", id = self.id, skill = name}
	end
end
