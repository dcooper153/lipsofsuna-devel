require "common/spec"

Feat = Class(Spec)
Feat.type = "feat"
Feat.dict_id = {}
Feat.dict_cat = {}
Feat.dict_name = {}

--- Registers a new feat.
-- @param clss Feat class.
-- @param args Arguments.<ul>
--   <li>affects_allies: Applicable to allied creatures.</li>
--   <li>affects_enemies: Applicable to enemy creatures.</li>
--   <li>affects_items: Applicable to items.</li>
--   <li>affects_terrain: Applicable to terrain.</li>
--   <li>categories: List of categories.</li>
--   <li>cooldown: Cooldown time in seconds.</li>
--   <li>effect_animation: Animation name.</li>
--   <li>effect_sound: Sound effect name.</li>
--   <li>inflict_damage: Damage inflicted to affected objects.</li>
--   <li>inflict_modifier: Modifier inflicted to affected objects.</li>
--   <li>locked: Not unlocked yet.</li>
--   <li>name: Name of the feat. (required)</li>
--   <li>range: Maximum firing range for bullet and ray targeting modes.</li>
--   <li>radius: Area of effect radius.</li>
--   <li>required_ammo: Table of required ammo.</li>
--   <li>required_reagents: Table of required reagents.</li>
--   <li>required_skills: Table of required skills.</li>
--   <li>targeting_mode: The way how the target of the feat is selected.</li>
--   <li>toggle: True to trigger the handler on the key release event as well.</li></ul>
-- @return New feat.
Feat.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.skills = self.skills or {}
	self.affects_allies = self.affects_allies or false
	self.affects_enemies = self.affects_enemies or false
	self.affects_items = self.affects_items or false
	self.affects_terrain = self.affects_terrain or false
	self.cooldown = self.cooldown or 0
	self.inflict_damage = self.inflict_damage or 0
	self.inflict_modifier = self.inflict_modifier
	self.locked = self.locked or false
	self.range = self.range or 0
	self.radius = self.radius or 0
	self.required_ammo = self.required_ammo or {}
	self.required_reagents = self.required_reagents or {}
	self.required_skills = self.required_skills or {}
	self.targeting_mode = self.targeting_mode or "none"
	self.toggle = self.toggle or false
	return self
end
