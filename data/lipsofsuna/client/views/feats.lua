require "client/quickslots"

Views.Feats = Class()

--- Creates a new feat editor.
-- @param clss Feats class.
-- @return Feats.
Views.Feats.new = function(clss)
	local self = Class.new(clss)
	self.anim = "spell on self"
	self.effects = {}
	self.slot = 1
	self.skills_text = ""
	self.reagents_text = ""
	self:show(1)
	return self
end

Views.Feats.add_effect = function(self, effect)
	for j = 1,3 do
		local e = self.effects[j]
		if not e then
			self.effects[j] = effect
			self:changed()
			return
		end
	end
end

Views.Feats.assign = function(self)
	-- Get effects and their magnitudes.
	local effects = {}
	for i = 1,3 do
		local e = self.effects[i]
		if e then
			table.insert(effects, {e.name, 1}) -- FIXME
		end
	end
	-- Create a feat from the animation and the effects.
	if self.anim and #effects > 0 then
		local feat = Feat{animation = self.anim, effects = effects}
		Quickslots:assign_feat(self.slot, feat)
	else
		Quickslots:assign_feat(self.slot)
	end
end

--- Recalculates the skill and reagent requirements of the currently shown feat.
-- @param self Feats.
Views.Feats.changed = function(self)
	-- Get effects and their magnitudes.
	local effects = {}
	local values = {}
	local both = {}
	for i = 1,3 do
		local e = self.effects[i]
		if e then
			table.insert(effects, e.name)
			table.insert(values, 1) -- FIXME
			table.insert(both, {e.name, 1})
		end
	end
	-- Calculate skill and reagent requirements.
	local feat = Feat{animation = self.anim, effects = both}
	local info = feat:get_info()
	if not info then
		self.skills_text = ""
		self.reagents_text = ""
		return
	end
	local reagents = info.required_reagents
	local skills = info.required_skills
	-- Display skill requirements.
	local skill_list = {}
	for k,v in pairs(skills) do
		table.insert(skill_list, k .. ": " .. v)
	end
	table.sort(skill_list)
	local skill_str = ""
	for k,v in ipairs(skill_list) do
		skill_str = skill_str .. (skill_str ~= "" and "\n" or "") .. v
	end
	self.skills_text = "Required skills:\n" .. skill_str
	-- Display reagent requirements.
	local reagent_list = {}
	for k,v in pairs(reagents) do
		table.insert(reagent_list, k .. ": " .. v)
	end
	table.sort(reagent_list)
	local reagent_str = ""
	for k,v in ipairs(reagent_list) do
		reagent_str = reagent_str .. (reagent_str ~= "" and "\n" or "") .. v
	end
	self.reagents_text = "Required reagents:\n" .. reagent_str
end

--- Gets the current feat animation name.
-- @param self Chargen.
Views.Feats.get_anim = function(self)
	return self.anim
end

--- Sets the feat animation name.
-- @param self Feats.
-- @param name Animation name.
Views.Feats.set_anim = function(self, name)
	self.anim = name
	for i = 1,3 do
		self.effects[i] = nil
	end
	self:changed()
end

--- Gets the current feat effect name.
-- @param self Chargen.
-- @param index Effect slot index.
Views.Feats.get_effect = function(self, index)
	local e = self.effects[index]
	return e and e.name
end

--- Sets the current feat effect.
-- @param self Chargen.
-- @param index Effect slot index.
-- @param value Effect or nil.
Views.Feats.set_effect = function(self, index, value)
	self.effects[index] = value
	self:changed()
end

--- Sets the race of the character using the feat editor.
-- @param self Feats.
-- @param name Race name.
Views.Feats.set_race = function(self, name)
	self.spec = Species:find{name = name}
	self:set_anim(self.anim)
end

--- Shows the feat for the given quickslot.
-- @param self Feats.
-- @param index Quickslot index.
Views.Feats.show = function(self, index)
	local feat = Quickslots.feats.buttons[index].feat or Feat()
	self.slot = index
	self.anim = feat.animation
	for j = 1,3 do
		local e = feat.effects[j]
		local s = e and Feateffectspec:find{name = e[1]}
		self.effects[j] = s
	end
	self:changed()
end
