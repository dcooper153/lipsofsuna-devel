Views.Skills = Class(Widget)

Views.Skills.getter = function(self, key, value)
	if key == "total" then
		local t = 0
		for k,v in pairs(self.dict_id) do t = t + v.value end
		return t
	else
		return Widget.getter(self, key, value)
	end
end

--- Creates a new skills widget.
-- @param clss skills class.
-- @param args Arguments.<ul>
--   <li>main: Synchronize skills with the main window.</li>
--   <li>sync: Synchronize skills with the server.</li></ul>
-- @return Skills widget.
Views.Skills.new = function(clss, args)
	local self = Widget.new(clss, {cols = 2, rows = 2, spacings = {40, 5},
		main = args and args.main, sync = args and args.sync})
	self.dict_id = {}
	self.dict_row = {}
	-- Create widgets.
	self.list = Widgets.List{pressed = function(view, row) self:show(row) end}
	self.skill_name = Widgets.Label{font = "medium"}
	self.skill_desc = Widgets.Label()
	self.skill_desc:set_request{width = 300}
	self.group = Widget{cols = 1, rows = 3}
	self.group:set_expand{row = 3}
	self.group:set_child{col = 1, row = 1, widget = self.skill_name}
	self.group:set_child{col = 1, row = 2, widget = self.skill_desc}
	self:set_expand{col = 2, row = 1}
	self:set_child{col = 1, row = 2, widget = self.list}
	self:set_child{col = 2, row = 2, widget = self.group}
	-- Create skills.
	self:add("dexterity", "Dexterity", "Determines the effectiveness of your ranged attacks, as well as how fast you can move and react.")
	self:add("health", "Health", "Determines how much damage your can withstand.")
	self:add("intelligence", "Intelligence", "Determines what items you can craft and how effectively you can handle technology based items.")
	self:add("perception", "Perception", "Determines how far you can see and how well you can handle precision weapons and items.")
	self:add("strength", "Strength", "Determines your physical power and the effectiveness of your melee attacks.")
	self:add("willpower", "Willpower", "Determines what spells you can cast and how effective they are.")
	self:show(1)
	return self
end

--- Adds a skill.
-- @param clss Skills.
-- @param id Skill id.
-- @param name Skill name.
-- @param desc Skill description.
Views.Skills.add = function(self, id, name, desc)
	local index = #self.dict_row + 1
	local skill = Widgets.SkillControl{
		cap = 0, desc = desc, id = id, index = index,
		max = 100, name = name, text = name, value = 0,
		pressed = function(w)
			local v = w:get_value_at(Client.cursor_pos)
			if self.species then v = math.min(v, self.species.skill_quota + w.value - self.total) end
			w.cap = v
			if not self.sync then w.value = v end
			self:show(w.index)
			self:changed(w.index)
		end}
	self.dict_id[id] = skill
	self.dict_row[index] = skill
	self.list:append_row(skill)
end

Views.Skills.back = function(self)
	Gui:set_mode("menu")
end

--- Sends the value of the currently selected skill to the server.
-- @param self Skills.
-- @param index Skill index.
Views.Skills.changed = function(self, index)
	if not self.sync then return end
	local skill = self.dict_row[index]
	if not skill then return end
	Network:send{packet = Packet(packets.SKILLS, "string", skill.id, "float", skill.cap)}
end

Views.Skills.get_value = function(self, id)
	return self.dict_id[id].value
end

--- Sets the species for which the skills are.
-- @param self Skills.
-- @param value Species.
Views.Skills.set_species = function(self, value)
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
-- @param self Skills.
-- @param index Skill number.
Views.Skills.show = function(self, index)
	local skill = self.dict_row[index]
	local species = Species:find{name = Player.species}
	local spec = species and species.skills[skill.id]
	self.skill_name.text = skill.name
	self.skill_desc.text = skill.desc
end

--- Updates a skill.
-- @param self Skills.
-- @param id Skill id.
-- @param value Current value.
-- @param cap Custom cap.
Views.Skills.update = function(self, id, value, cap)
	local skill = self.dict_id[id]
	if skill then
		local species = Species:find{name = Player.species}
		local spec = species and species.skills[skill.id]
		skill.value = value
		skill.cap = cap
		skill.max = spec and spec.max or 100
		if self.main then
			if id == "health" then
				Gui.skill_health.cap = cap
				Gui.skill_health.value = value
			elseif id == "willpower" then
				Gui.skill_mana.cap = cap
				Gui.skill_mana.value = value
			end
		end
	end
end
