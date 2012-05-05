require(Mod.path .. "spec")

Actorspec = Class(Spec)
Actorspec.type = "species"
Actorspec.dict_id = {}
Actorspec.dict_cat = {}
Actorspec.dict_name = {}
Actorspec.introspect = Introspect{
	name = "Actorspec",
	base = function(spec) return Actorspec:find{name = spec.base} end,
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "base", type = "string", description = "Base actor spec.", details = {spec = "Actorspec"}},
		{name = "ai_enable_attack", type = "boolean", default = true, description = "False to prohibit attacking."},
		{name = "ai_enable_backstep", type = "boolean", default = true, description = "False to prohibit backstep."},
		{name = "ai_enable_block", type = "boolean", default = true, description = "False to prohibit blocking."},
		{name = "ai_enable_combat", type = "boolean", default = true, description = "False to prohibit combat."},
		{name = "ai_enable_jump", type = "boolean", default = true, description = "False to prohibit jumping."},
		{name = "ai_enable_spells", type = "boolean", default = true, description = "False to prohibit spell casting."},
		{name = "ai_enable_strafe", type = "boolean", default = true, description = "False to prohibit strafing."},
		{name = "ai_enable_wander", type = "boolean", default = true, description = "False to prohibit wandering."},
		{name = "ai_enable_walk", type = "boolean", default = true, description = "False to prohibit walking."},
		{name = "ai_enable_weapon_switch", type = "boolean", default = true, description = "False to prohibit weapon switching."},
		{name = "ai_enabled", type = "boolean", default = true, description = "False to disable AI for the actor."},
		{name = "ai_offense_factor", type = "number", default = 0.75, description = "How much the creature prefers offense over defense, in the range of [0,1]."},
		{name = "ai_search_time", type = "number", default = 20, description = "Time in seconds how long the AI will search for hidden foes."},
		{name = "ai_type", type = "string", description = "Name of the AI type to use."},
		{name = "ai_update_delay", type = "number", default = 2, description = "Number of seconds between AI state updates."},
		{name = "ai_wait_allowed", type = "boolean", default = true, description = "False to prohibit waiting."},
		{name = "ai_wander_time", type = "number", default = 60, description = "Time in seconds how long the AI will wander before going to sleep."},
		{name = "aim_ray_center", type = "vector", default = Vector(0,1), description = "Center vector of the aim ray."},
		{name = "aim_ray_end", type = "number", default = 5, description = "Aim ray end distance."},
		{name = "aim_ray_start", type = "number", default = 0.1, description = "Aim ray start distance."},
		{name = "animations", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of animations.", details = {values = {spec = "Animationspec"}}},
		{name = "blocking_armor", type = "number", default = 0.5, description = "How much armor class blocking offsers."},
		{name = "blocking_cooldown", type = "number", default = 0.4, description = "Time in seconds how long it takes to leave the blocking stance."},
		{name = "blocking_delay", type = "number", default = 0.4, description = "Time in seconds how long it takes to enter the blocking stance."},
		{name = "body_scale_min", type = "number", default = 1, description = "Minimum body scale."},
		{name = "body_scale_max", type = "number", default = 1, description = "Maximum body scale."},
		{name = "camera_center", type = "vector", description = "Camera center position."},
		{name = "collision_group", type = "number", default = 0x0001},
		{name = "damage_from_magma", type = "number", default = 6, description = "Points of damage from magma per second."},
		{name = "damage_from_water", type = "number", default = 0, description = "Points of damage from water per second."},
		{name = "dead", type = "boolean", description = "True if the creature should spawn as dead."},
		{name = "dialog", type = "string", description = "Dialog name.", details = {spec = "Dialogspec"}},
		{name = "difficulty", type = "number", default = 0, description = "The approximate difficulty of the creature in the range of [0,1]."},
		{name = "effect_falling_damage", type = "string", description = "Name of the effect played when the creature takes falling damage."},
		{name = "effect_landing", type = "string", description = "Name of the effect played when the creature lands after jumping."},
		{name = "effect_physical_damage", type = "string", description = "Name of the effect played when the creature is hurt physically."},
		{name = "equipment_class", type = "string", description = "Name of the equipment class to use for equipment models."},
		{name = "equipment_slots", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of equipment slots."},
		{name = "eye_color", type = "color", description = "Eye color."},
		{name = "eye_style", type = "string", description = "Eye style."},
		{name = "eye_styles", type = "dict", dict = {type = "string"}, description = "Dictionary of eye styles."},
		{name = "factions", type = "dict", dict = {type = "boolean"}, default = {}, description = "List of factions.", details = {keys = {spec = "Factionspec"}}},
		{name = "falling_damage_rate", type = "number", default = 10, description = "Number of points of damage per every meters per second exceeding the falling damage speed."},
		{name = "falling_damage_speed", type = "number", default = 10, description = "Speed in meters per seconds after which the creature starts taking falling damage."},
		{name = "feat_types", type = "dict", dict = {type = "boolean"}, default = {}, description = "List of know feat types.", details = {keys = {spec = "Feattypespec"}}},
		{name = "feat_effects", type = "dict", dict = {type = "boolean"}, default = {}, description = "List of know feat effects.", details = {keys = {spec = "Feateffectspec"}}},
		{name = "footstep_height", type = "number", description = "Footstep height."},
		{name = "footstep_sound", type = "string", description = "Name of the footstep sound effect."},
		{name = "gravity", type = "vector", default = Vector(0,-15), description = "Gravity vector."},
		{name = "hair_color", type = "color", description = "Hair color."},
		{name = "hair_style", type = "string", description = "Hair style."},
		{name = "hair_styles", type = "dict", dict = {type = "string"}, description = "Dictionary of hair styles."},
		{name = "interactive", type = "boolean", default = true, description = "False to make the object not appear interactive."},
		{name = "inventory_items", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of inventory items to give when the creature is spawned.", details = {keys = {spec = "Itemspec"}, values = {integer = true, min = 1}}},
		{name = "inventory_size", type = "number", default = 0, description = "Number of inventory slots the creature has."},
		{name = "jump_force", type = "number", default = 8, description = "Mass-independent jump force of the creature."},
		{name = "loot_categories", type = "list", list = {type = "string"}, description = "List of item categories this creature can have as random loot."},
		{name = "loot_count_min", type = "number", description = "Minimum number of random loot items this actor can have."},
		{name = "loot_count_max", type = "number", description = "Maximum number of random loot items this actor can have."},
		{name = "personality", type = "string", description = "Name of the personality type.", details = {spec = "Personalityspec"}},
		{name = "preset", type = "string", description = "Name of the actor preset to use.", details = {spec = "Actorpresetspec"}},
		{name = "marker", type = "string", description = "Map marker name."},
		{name = "mass", type = "number", default = 50, description = "Mass in kilograms."},
		{name = "model", type = "string", description = "Model name."},
		{name = "models", type = "dict", dict = {type = "string"}, description = "Dictionary or model names."},
		{name = "skill_regen", type = "number", default = 0.5, description = "Skill regeneration speed in units per second."},
		{name = "skill_quota", type = "number", default = 200, description = "Number of skill points the creature can distribute over skills."},
		{name = "skills", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of skills.", details = {keys = {spec = "Skillspec"}}},
		{name = "skin_color", type = "color", description = "Skin color."},
		{name = "skin_material", type = "string", description = "Skin material name."},
		{name = "skin_styles", type = "dict", dict = {type = "string"}, description = "Dictionary of skin styles."},
		{name = "skin_textures", type = "list", list = {type = "string"}, description = "List of skin textures."},
		{name = "special_effects", type = "list", list = {type = "string", details = {value = {spec = "Effectspec"}}}, default = {}, description = "List of special effects to render."},
		{name = "speed_run", type = "number", default = 6, description = "Turning speed in meters per second."},
		{name = "speed_walk", type = "number", default = 3, description = "Walking speed in meters per second."},
		{name = "swim_force", type = "number", default = 3.5, description = "Mass-independent upward swim force of the creature."},
		{name = "tilt_bone", type = "list", list = {type = "string"}, default = {}, description = "Name of the tilt bone of the creature."},
		{name = "tilt_limit", type = "number", default = math.pi, description = "Tilt limit in radians of the tilting bone."},
		{name = "timing_attack_bow", type = "number", default = 20, description = "Timing of bow firing, in frames."},
		{name = "timing_attack_crossbow", type = "number", default = 20, description = "Timing of crossbow firing, in frames."},
		{name = "timing_attack_explode", type = "number", default = 120, description = "Timing of explosion attack, in frames."},
		{name = "timing_attack_melee", type = "number", default = 20, description = "Timing of melee attack impact, in frames."},
		{name = "timing_attack_musket", type = "number", default = 20, description = "Timing of musket firing, in frames."},
		{name = "timing_attack_revolver", type = "number", default = 20, description = "Timing of revolver firing, in frames."},
		{name = "timing_attack_throw", type = "number", default = 20, description = "Timing of releasing the thrown item, in frames."},
		{name = "timing_build", type = "number", default = 40, description = "Timing of releasing the thrown item, in frames."},
		{name = "timing_drop", type = "number", default = 20, description = "Timing of dropping an item, in frames."},
		{name = "timing_jump", type = "number", default = 20, description = "Timing of lifting off when jumping, in frames."},
		{name = "timing_pickup", type = "number", default = 40, description = "Timing of picking up an item, in frames."},
		{name = "timing_spell_ranged", type = "number", default = 40, description = "Timing of casting a ranged spell projectile, in frames."},
		{name = "timing_spell_self", type = "number", default = 40, description = "Timing of casting a spell on self, in frames."},
		{name = "timing_spell_touch", type = "number", default = 40, description = "Timing of casting a spell on touch, in frames."},
		{name = "view_cone", type = "number", default = 0.8 * math.pi, description = "View cone angle in radians."},
		{name = "vulnerabilities", type = "dict", dict = {type = "number"}, description = "Dictionary of damage vulnerabilities."},
		{name = "weapon_slot", type = "string", default = "hand.R", description = "Name of the weapon slot."},
		{name = "water_friction", type = "number", default = 0.8, description = "How much being in water slows the creature down."},
		{name = "water_gravity", type = "vector", default = Vector(0,-1), description = "The gravity of the creature in water."}
	}}

-- FIXME
Species = Actorspec

--- Registers a new actor spec.
-- @param clss Actorspec class.
-- @param args Arguments.
-- @return New actor spec.
Actorspec.new = function(clss, args)
	-- Copy arguments from the base spec.
	-- The arguments used to initialize each spec have been stored to the
	-- args field of the instance so that they can be easily copied to the
	-- inherited specs here.
	local base = args.base and clss:find{name = args.base}
	if base then
		local t = {}
		for k,v in pairs(base.args) do t[k] = v end
		for k,v in pairs(args) do t[k] = v end
		args = t
	end
	-- Allocate the spec.
	-- The argument list is stored to allow easy inheritance.
	local self = Spec.new(clss, args)
	self.args = args
	self.introspect:read_table(self, args)
	-- Animations.
	-- Inheritance of animations is special in that they're inherited
	-- in per animation basis instead of the usual all or none inheritance.
	self.animations = {}
	if base and base.animations then
		for k,v in pairs(base.animations) do
			self.animations[k] = v
		end
	end
	if args.animations then
		for k,v in pairs(args.animations) do
			self.animations[k] = v
		end
	end
	-- Precalculate combat abilities.
	self.can_melee = false
	self.can_ranged = false
	self.can_throw = false
	self.can_cast_ranged = false
	self.can_cast_self = false
	self.can_cast_touch = false
	for k,v in pairs(self.feat_types) do
		local feat = Feattypespec:find{name = k}
		if self.ai_enable_attack then
			if feat.categories["melee"] then self.can_melee = true end
			if feat.categories["ranged"] then self.can_ranged = true end
			if feat.categories["throw"] then self.can_throw = true end
		end
		if self.ai_enable_spells then
			if feat.categories["ranged spell"] then self.can_cast_ranged = true end
			if feat.categories["spell on self"] then self.can_cast_self = true end
			if feat.categories["spell on touch"] then self.can_cast_touch = true end
		end
	end
	return self
end

--- Checks if the object is an enemy of the actor.
-- @param self Actor spec.
-- @param object Object.
-- @return True if an enemy.
Actorspec.check_enemy = function(self, object)
	if object.spec.type ~= "species" then return end
	for name1 in pairs(self.factions) do
		local spec1 = Factionspec:find{name = name1}
		if spec1 then
			for name2 in pairs(object.spec.factions) do
				if spec1.enemies[name2] then
					return true
				end
			end
		end
	end
end

--- Gets an animation by name.
-- @param self Actor spec.
-- @param name Animation name.
-- @return Animation spec, or nil.
Actorspec.get_animation = function(self, name)
	local n = self.animations[name]
	if not n then return end
	return Animationspec:find{name = n}
end

--- Gets the personality of the actor.
-- @param self Actor spec.
-- @return Personality spec, or nil.
Actorspec.get_personality = function(self)
	if not self.personality then return end
	if not Personalityspec then return end
	return Personalityspec:find{name = self.personality}
end

--- Gets a random eye style for the actor.
-- @param self Actor spec.
-- @return Table containing the style and color, or nil.
Actorspec.get_random_eyes = function(self)
	-- Choose the style.
	local style = self.eye_style
	if not style then return end
	if style == "random" then
		if not self.eye_styles then return end
		local lst = {}
		for k,v in pairs(self.eye_styles) do
			table.insert(lst, v)
		end
		local l = #self.eye_styles
		if l == 0 then return end
		style = lst[math.random(1, l)]
	end
	-- Choose the color.
	local color = self.eye_color
	if not color then
		color = Color:hsv_to_rgb{math.random(), 0.2 + 0.8 * math.random(), math.random()}
		color[1] = math.floor(255 * color[1] + 0.5)
		color[2] = math.floor(255 * color[1] + 0.5)
		color[3] = math.floor(255 * color[1] + 0.5)
	end
	-- Return the style table.
	return {style, color[1], color[2], color[3]}
end

--- Gets a random hair style for the actor.
-- @param self Actor spec.
-- @return Table containing the style and color, or nil.
Actorspec.get_random_hair = function(self)
	-- Choose the style.
	local style = self.hair_style
	if not style then return end
	if style == "random" then
		if not self.hair_styles then return end
		local lst = {}
		for k,v in pairs(self.hair_styles) do
			table.insert(lst, v)
		end
		local l = #self.eye_styles
		if l == 0 then return end
		style = lst[math.random(1, l)]
	end
	-- Choose the color.
	local color = self.hair_color
	if not color then
		color = {}
		color[1] = math.random(0, 255)
		color[2] = math.random(0, 255)
		color[3] = math.random(0, 255)
	end
	-- Return the style table.
	return {style, color[1], color[2], color[3]}
end

--- Finds the special effects of the actor.
-- @param self Actor spec.
-- @return List of effect specs, or nil.
Actorspec.get_special_effects = function(self)
	if not self.special_effects then return end
	local res = {}
	for k,v in pairs(self.special_effects) do
		local eff = Effectspec:find{name = v}
		if eff then
			table.insert(res, eff)
		end
	end
	if #res == 0 then return end
	return res
end

--- Sets the factions of the actor.
-- @param self Actor spec.
-- @param args List of factions.
Actorspec.set_factions = function(self, args)
	if args then
		self.factions = {}
		for k,v in pairs(args) do
			self.factions[v] = Factionspec:find{name = v}
		end
	end
end
