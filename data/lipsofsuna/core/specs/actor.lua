require(Mod.path .. "spec")

Actorspec = Class(Spec)
Actorspec.type = "actor"
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
		{name = "ai_combat_actions", type = "dict", dict = {type = "boolean"}, default = {["block"] = true, ["idle"] = true, ["melee"] = true, ["move backward"] = true, ["move forward"] = true, ["ranged"] = true, ["ranged spell"] = true, ["spell on self"] = true, ["strafe"] = true, ["switch weapon"] = true, ["throw"] = true, ["touch spell"] = true}, description = "Dictionary of allowed combat actions", details = {keys = {spec = "Aiactionspec"}}},
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
		{name = "ai_enabled_states", type = "dict", dict = {type = "boolean"}, default = {["chat"] = true, ["combat"] = true, ["flee"] = true, ["follow"] = true, ["hide"] = true, ["none"] = true, ["search"] = true, ["wait"] = true, ["wander"] = true}, description = "Dictionary of allowed combat actions", details = {keys = {spec = "Aistatespec"}}},
		{name = "ai_offense_factor", type = "number", default = 0.75, description = "How much the actor prefers offense over defense, in the range of [0,1]."},
		{name = "ai_search_time", type = "number", default = 20, description = "Time in seconds how long the AI will search for hidden foes."},
		{name = "ai_type", type = "string", description = "Name of the AI type to use."},
		{name = "ai_update_delay", type = "number", default = 2, description = "Number of seconds between AI state updates."},
		{name = "ai_wait_allowed", type = "boolean", default = true, description = "False to prohibit waiting."},
		{name = "ai_wander_time", type = "number", default = 60, description = "Time in seconds how long the AI will wander before going to sleep."},
		{name = "aim_ray_center", type = "vector", default = Vector(0,1), description = "Center vector of the aim ray."},
		{name = "aim_ray_end", type = "number", default = 5, description = "Aim ray end distance."},
		{name = "aim_ray_start", type = "number", default = 0.1, description = "Aim ray start distance."},
		{name = "animations", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of animation profiles.", details = {values = {spec = "AnimationProfileSpec"}}},
		{name = "blocking_armor", type = "number", default = 0.5, description = "How much armor class blocking offsers."},
		{name = "blocking_cooldown", type = "number", default = 0.4, description = "Time in seconds how long it takes to leave the blocking stance."},
		{name = "blocking_delay", type = "number", default = 0.4, description = "Time in seconds how long it takes to enter the blocking stance."},
		{name = "body_scale_min", type = "number", default = 1, description = "Minimum body scale."},
		{name = "body_scale_max", type = "number", default = 1, description = "Maximum body scale."},
		{name = "camera_center", type = "vector", description = "Camera center position."},
		{name = "collision_group", type = "number", default = 0x0001},
		{name = "damage_from_magma", type = "number", default = 6, description = "Points of damage from magma per second."},
		{name = "damage_from_water", type = "number", default = 0, description = "Points of damage from water per second."},
		{name = "dead", type = "boolean", description = "True if the actor should spawn as dead."},
		{name = "dialog", type = "string", description = "Dialog name.", details = {spec = "Dialogspec"}},
		{name = "difficulty", type = "number", default = 0, description = "The approximate difficulty of the actor in the range of [0,1]."},
		{name = "effect_falling_damage", type = "string", description = "Name of the effect played when the actor takes falling damage."},
		{name = "effect_landing", type = "string", description = "Name of the effect played when the actor lands after jumping."},
		{name = "effect_physical_damage", type = "string", description = "Name of the effect played when the actor is hurt physically."},
		{name = "equipment_class", type = "string", description = "Name of the equipment class to use for equipment models."},
		{name = "equipment_slots", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of equipment slots."},
		{name = "eye_color", type = "color", description = "Eye color."},
		{name = "eye_style", type = "string", description = "Eye style."},
		{name = "eye_styles", type = "dict", dict = {type = "string"}, description = "Dictionary of eye styles."},
		{name = "factions", type = "dict", dict = {type = "boolean"}, default = {}, description = "List of factions.", details = {keys = {spec = "Factionspec"}}},
		{name = "falling_damage_rate", type = "number", default = 10, description = "Number of points of damage per every meters per second exceeding the falling damage speed."},
		{name = "falling_damage_speed", type = "number", default = 10, description = "Speed in meters per seconds after which the actor starts taking falling damage."},
		{name = "feat_types", type = "dict", dict = {type = "boolean"}, default = {}, description = "List of know feat types.", details = {keys = {spec = "Feattypespec"}}},
		{name = "feat_effects", type = "dict", dict = {type = "boolean"}, default = {}, description = "List of know feat effects.", details = {keys = {spec = "Feateffectspec"}}},
		{name = "footstep_height", type = "number", description = "Footstep height."},
		{name = "footstep_sound", type = "string", description = "Name of the footstep sound effect."},
		{name = "gravity", type = "vector", default = Vector(0,-15), description = "Gravity vector."},
		{name = "hair_color", type = "color", description = "Hair color."},
		{name = "hair_style", type = "string", description = "Hair style."},
		{name = "hair_styles", type = "dict", dict = {type = "string"}, description = "Dictionary of hair styles."},
		{name = "head_styles", type = "dict", dict = {type = "string"}, description = "Dictionary of head styles."},
		{name = "important", type = "boolean", description = "True to not allow the actor to be cleaned up from the map."},
		{name = "interactive", type = "boolean", default = true, description = "False to make the object not appear interactive."},
		{name = "inventory_items", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of inventory items to give when the actor is spawned.", details = {keys = {spec = "Itemspec"}, values = {integer = true, min = 1}}},
		{name = "inventory_items_death", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of inventory items to give when the actor is killed.", details = {keys = {spec = "Itemspec"}, values = {integer = true, min = 1}}},
		{name = "inventory_size", type = "number", default = 0, description = "Number of inventory slots the actor has."},
		{name = "jump_force", type = "number", default = 8, description = "Mass-independent jump force of the actor."},
		{name = "loot_categories", type = "list", list = {type = "string"}, description = "List of item categories this actor can have as random loot."},
		{name = "loot_count_min", type = "number", description = "Minimum number of random loot items this actor can have."},
		{name = "loot_count_max", type = "number", description = "Maximum number of random loot items this actor can have."},
		{name = "personality", type = "string", description = "Name of the personality type.", details = {spec = "Personalityspec"}},
		{name = "preset", type = "string", description = "Name of the actor preset to use.", details = {spec = "Actorpresetspec"}},
		{name = "marker", type = "string", description = "Map marker name."},
		{name = "mass", type = "number", default = 50, description = "Mass in kilograms."},
		{name = "model", type = "string", description = "Model name."},
		{name = "models", type = "dict", dict = {type = "string"}, description = "Dictionary or model names."},
		{name = "reagentless_spells", type = "boolean", description = "True to allow the actor to cast spells without reagents."},
		{name = "skill_regen", type = "number", default = 0.5, description = "Skill regeneration speed in units per second."},
		{name = "skill_quota", type = "number", default = 200, description = "Number of skill points the actor can distribute over skills."},
		{name = "skills", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of skills.", details = {keys = {spec = "Skillspec"}}},
		{name = "skin_color", type = "color", description = "Skin color."},
		{name = "skin_material", type = "string", description = "Skin material name."},
		{name = "skin_styles", type = "dict", dict = {type = "string"}, description = "Dictionary of skin styles."},
		{name = "skin_textures", type = "list", list = {type = "string"}, description = "List of skin textures."},
		{name = "special_effects", type = "list", list = {type = "string", details = {value = {spec = "Effectspec"}}}, default = {}, description = "List of special effects to render."},
		{name = "speed_run", type = "number", default = 6, description = "Turning speed in meters per second."},
		{name = "speed_walk", type = "number", default = 3, description = "Walking speed in meters per second."},
		{name = "swim_force", type = "number", default = 3.5, description = "Mass-independent upward swim force of the actor."},
		{name = "tilt_bone", type = "list", list = {type = "string"}, default = {}, description = "Name of the tilt bone of the actor."},
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
		{name = "water_friction", type = "number", default = 0.8, description = "How much being in water slows the actor down."},
		{name = "water_gravity", type = "vector", default = Vector(0,-1), description = "The gravity of the actor in water."}
	}}

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
	if object.spec.type ~= "actor" then return end
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
-- @param profile Animation profile mapping, or nil for "default".
-- @return Animation spec, or nil.
Actorspec.get_animation = function(self, name, profile)
	local try = function(self, p, a)
		local pname = self.animations[p]
		if not pname then return end
		local profile = AnimationProfileSpec:find{name = pname}
		if not profile then return end
		return profile:get_animation(a)
	end
	return profile and try(self, profile, name) or try(self, "default", name)
end

--- Gets animation playback arguments by name.
-- @param self Actor spec.
-- @param name Animation name.
-- @param profile Animation profile mapping, or nil for "default".
-- @return Table of animation playback arguments.
Actorspec.get_animation_arguments = function(self, name, profile)
	local args = {animation = name, fade_in = 0.3, fade_out = 0.3, time = time}
	local anim = self:get_animation(name, profile)
	if anim then
		for k,v in pairs(anim:get_arguments()) do args[k] = v end
	end
	return args
end

--- Gets a model node name for the given equipment slot.
-- @param self Actor spec.
-- @param slot Equipment slot name.
-- @return Node name, or nil.
Actorspec.get_node_by_equipment_slot = function(self, slot)
	if not self.equipment_slots then return end
	local node = self.equipment_slots[slot]
	if node == "" then return end
	return node
end

--- Gets the personality of the actor.
-- @param self Actor spec.
-- @return Personality spec, or nil.
Actorspec.get_personality = function(self)
	if not self.personality then return end
	if not Personalityspec then return end
	return Personalityspec:find{name = self.personality}
end

--- Gets a random eye color for the actor.
-- @param self Actor spec.
-- @return Color table, or nil.
Actorspec.get_random_eye_color = function(self)
	if not self.eye_style then return end
	local color = self.eye_color
	if not color then
		color = Color:hsv_to_rgb{math.random(), 0.2 + 0.8 * math.random(), math.random()}
		color[1] = math.floor(255 * color[1] + 0.5)
		color[2] = math.floor(255 * color[1] + 0.5)
		color[3] = math.floor(255 * color[1] + 0.5)
	end
	return {color[1], color[2], color[3]}
end

--- Gets a random eye style for the actor.
-- @param self Actor spec.
-- @return Style name, or nil.
Actorspec.get_random_eye_style = function(self)
	local style = self.eye_style
	if not style then return end
	if style == "random" then
		if not self.eye_styles then return end
		local lst = {}
		for k,v in pairs(self.eye_styles) do
			table.insert(lst, v)
		end
		local l = #lst
		if l == 0 then return end
		style = lst[math.random(1, l)]
	end
	return style
end

--- Gets a random hair color for the actor.
-- @param self Actor spec.
-- @return Color table, or nil.
Actorspec.get_random_hair_color = function(self)
	if not self.hair_style then return end
	local color = self.hair_color
	if not color then
		color = {}
		color[1] = math.random(0, 255)
		color[2] = math.random(0, 255)
		color[3] = math.random(0, 255)
	end
	return {color[1], color[2], color[3]}
end

--- Gets a random hair style for the actor.
-- @param self Actor spec.
-- @return Style string, or nil.
Actorspec.get_random_hair_style = function(self)
	local style = self.hair_style
	if not style then return end
	if style == "random" then
		if not self.hair_styles then return end
		local lst = {}
		for k,v in pairs(self.hair_styles) do
			table.insert(lst, v)
		end
		local l = #lst
		if l == 0 then return end
		style = lst[math.random(1, l)]
	end
	return style
end

--- Gets a random head style for the actor.
-- @param self Actor spec.
-- @return Head style name, or nil.
Actorspec.get_random_head = function(self)
	local style = self.head_style
	if not style then return end
	if style == "random" then
		if not self.head_styles then return end
		local lst = {}
		for k,v in pairs(self.head_styles) do
			table.insert(lst, v)
		end
		local l = #lst
		if l == 0 then return end
		style = lst[math.random(1, l)]
	end
	return style
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
