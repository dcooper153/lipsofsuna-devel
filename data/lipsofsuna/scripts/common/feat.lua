require "common/spec"

Featanimspec = Class(Spec)
Featanimspec.type = "featanim"
Featanimspec.dict_id = {}
Featanimspec.dict_cat = {}
Featanimspec.dict_name = {}

--- Registers a feat animation.
-- @param clss Featanimspec class.
-- @param args Arguments.<ul>
--   <li>action_frames: Blender frame range of the action portion.</li>
--   <li>categories: List of categories.</li>
--   <li>cooldown: Cooldown time in seconds.</li>
--   <li>effect_animation: Animation name.</li>
--   <li>effect_sound: Sound effect name.</li>
--   <li>icon: Icon name.</li>
--   <li>influences: List of {type, base} influences.</li>
--   <li>required_ammo: Table of required ammo.</li>
--   <li>toggle: True to trigger the handler on the key release event as well.</li></ul>
-- @return New feat animation.
Featanimspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.cooldown = self.cooldown or 0
	self.action_frames = self.action_frames or {2, 10}
	self.icon = self.icon or "skill-todo"
	self.influences = self.influences or {}
	self.required_ammo = self.required_ammo or {}
	self.toggle = self.toggle or false
	return self
end

Feateffectspec = Class(Spec)
Feateffectspec.type = "feateffect"
Feateffectspec.dict_id = {}
Feateffectspec.dict_cat = {}
Feateffectspec.dict_name = {}

--- Registers a feat effect.
-- @param clss Feateffectspec class.
-- @param args Arguments.<ul>
--   <li>affects_allies: Applicable to allied creatures.</li>
--   <li>affects_enemies: Applicable to enemy creatures.</li>
--   <li>affects_items: Applicable to items.</li>
--   <li>affects_terrain: Applicable to terrain.</li>
--   <li>categories: List of categories.</li>
--   <li>cooldown_base: Base cooldown time.</li>
--   <li>cooldown_mult: Cooldown time multiplier.</li>
--   <li>effect: Effect name.</li>
--   <li>influences: List of {type, base, mult} influences.</li>
--   <li>locked: True for not unlocked yet.</li>
--   <li>name: Skill effect name.</li>
--   <li>radius: Area of effect radius.</li>
--   <li>range: Maximum firing range for bullet and ray targeting modes.</li>
--   <li>skill_base: List of base skill requirements.</li>
--   <li>skill_mult: List of skill requirements multipliers.</li>
--   <li>reagent_base: List of base reagent requirements.</li>
--   <li>reagent_mult: List of reagent requirements multipliers.</li></ul>
-- @return New feat effect.
Feateffectspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.cooldown_base = self.cooldown_base or 1
	self.cooldown_mult = self.cooldown_mult or 0
	self.influences = self.influences or {}
	self.radius = self.radius or 0
	self.range = self.range or 0
	self.reagent_base = self.reagent_base or {}
	self.reagent_mult = self.reagent_mult or {}
	self.skill_base = self.skill_base or {}
	self.skill_mult = self.skill_mult or {}
	return self
end

Feat = Class()

--- Creates a new feat.
-- @param clss Feat class.
-- @param args Arguments.<ul>
--   <li>animation: Feat animation name.</li>
--   <li>effects: List of effects and their magnitudes.</li></ul>
-- @return New feat.
Feat.new = function(clss, args)
	local self = Class.new(clss, args)
	self.effects = self.effects or {}
	return self
end

--- Gets the skill and reagent requirements of the feat.
-- @param self Feat.
-- @return Feat info table.
Feat.get_info = function(self)
	local damage = 0
	local reagents = {}
	local skills = {}
	local influences = {}
	local anim = Featanimspec:find{name = self.animation}
	local cooldown = anim and anim.cooldown
	if anim then
		-- Influence contribution.
		for _,influ in pairs(anim.influences) do
			local n = influ[1]
			local v = influ[2]
			influences[n] = (influences[n] or 0) + v
			if influences[n] == 0 then
				influences[n] = nil
			end
		end
		-- Effect contributions.
		for index,data in pairs(self.effects) do
			local effect = Feateffectspec:find{name = data[1]}
			if effect then
				-- Base skill requirements.
				for skill,value in pairs(effect.skill_base) do
					local val = skills[skill] or 0
					skills[skill] = val + value
				end
				-- Magnitude based skill requirements.
				for skill,mult in pairs(effect.skill_mult) do
					local val = skills[skill] or 0
					skills[skill] = val + mult * data[2]
				end
				-- Base reagent requirements.
				for reagent,value in pairs(effect.reagent_base) do
					local val = reagents[reagent] or 0
					reagents[reagent] = val + value
				end
				-- Magnitude based reagent requirements.
				for reagent,mult in pairs(effect.reagent_mult) do
					local val = reagents[reagent] or 0
					reagents[reagent] = val + mult * data[2]
				end
				-- Cooldown contribution.
				cooldown = cooldown + effect.cooldown_base + effect.cooldown_mult * data[2]
				-- Influence contribution.
				for _,influ in pairs(effect.influences) do
					local n = influ[1]
					local v = influ[2] + influ[3] * data[2]
					influences[n] = (influences[n] or 0) + v
					if influences[n] == 0 then
						influences[n] = nil
					end
				end
			end
		end
		for k,v in pairs(skills) do
			skills[k] = math.max(1, math.floor(v))
		end
		for k,v in pairs(reagents) do
			reagents[k] = math.max(1, math.floor(v))
		end
	end
	return {
		animation = anim,
		cooldown = cooldown,
		influences = influences,
		required_ammo = anim and anim.required_ammo,
		required_reagents = reagents,
		required_skills = skills,
		required_weapon = anim and anim.required_weapon}
end
