--- Item speficification.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.item
-- @alias Itemspec

local AnimationProfileSpec = require("core/specs/animation-profile")
local Class = require("system/class")
local EffectProfileSpec = require("core/specs/effect-profile")
local Spec = require("core/specs/spec")

--- Item speficification.
-- @type Itemspec
Itemspec = Spec:register("Itemspec", "item", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "actions", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of actions the object performs when wielded.", details = {values = {spec = "Actionspec"}}},
	{name = "ammo_type", type = "string", description = "Name of the ammunition item type.", details = {spec = "Itemspec"}},
	{name = "animations", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of animation profiles.", details = {values = {spec = "AnimationProfileSpec"}}},
	{name = "animations_equipped", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of animation profiles used by the actor when equipping the item.", details = {values = {spec = "AnimationProfileSpec"}}},
	{name = "armor_class", type = "number", default = 0, description = "How much protection the item offers when equipped."},
	{name = "book_text", type = "string", description = "Content of player readable items."},
	{name = "censorship_nodes", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of nodes that require censorship."},
	{name = "collision_group", type = "number", default = 0x0002, description = "Collision group."},
	{name = "construct_tile", type = "string", description = "Tile type to construct when used.", details = {spec = "TerrainMaterialSpec"}},
	{name = "construct_tile_count", type = "number", description = "Material count required by tile construction.", details = {integer = true, min = 1}},
	{name = "damage_mining", type = "number", description = "Damage the item takes from mining."},
	{name = "description", type = "string", description = "Description of the item."},
	{name = "destroy_actions", type = "list", list = {type = "string"}, default = {}, description = "List of actions to perform when the item is destroyed."},
	{name = "destroy_timer", type = "number", description = "Time in seconds after which to destruct when thrown."},
	{name = "dialog", type = "string", description = "Dialog name.", details = {spec = "Dialogspec"}},
	{name = "effect_attack_speedline", type = "boolean", description = "True to enable the speed line effect for attacks."},
	{name = "effects", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of effect profiles.", details = {values = {spec = "EffectProfileSpec"}}},
	{name = "equipment_anchor", type = "string", description = "Name of the node snapped to the hand of the actor when wielded."},
	{name = "equipment_anchor_position", type = "vector", description = "Positional displacement of the model when equipped."},
	{name = "equipment_anchor_rotation", type = "quaternion", description = "Rotational displacement of the model when equipped."},
	{name = "equipment_models", type = "dict", dict = {type = "dict", dict = {type = "string"}}, description = "Dictionary of equipment models."},
	{name = "equipment_priority", type = "number", default = 0, description = "The priority of equipment models and textures when worn."},
	{name = "equipment_slot", type = "string", description = "Equipment slot into which the item can be placed."},
	{name = "equipment_slots_reserved", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of equipment slots that the item reserves in addition to the main slot."},
	{name = "equipment_textures", type = "dict", dict = {type = "dict", dict = {type = "string"}}, description = "Dictionary of equipment texture blit instructions. The dictionary keys are the base texture names and the values the source texture names."},
	{name = "gravity", type = "vector", default = Vector(0,-15), description = "Gravity vector."},
	{name = "gravity_projectile", type = "vector", default = Vector(0,-15), description = "Gravity vector for projectile mode."},
	{name = "health", type = "number", description = "Number of hit points the item has."},
	{name = "important", type = "boolean", description = "True to not allow the item to be cleaned up from the map."},
	{name = "modifiers", type = "dict", dict = {type = "number"}, description = "Dictionary of ways how attacks with the weapon modify the target."},
	{name = "modifiers_bonus", type = "list", list = {type = "string"}, description = "List of skill-derived attribute names that affect modifiers."},
	{name = "icon", type = "string", details = {value_in_spec = "Iconspec"}, description = "Icon name."},
	{name = "interactive", type = "boolean", default = true, description = "False to make the object not appear interactive."},
	{name = "inventory_items", type = "dict", dict = {type = "number"}, default = {}, description = "List of inventory items for containers.", details = {keys = {spec = "Itemspec"}, values = {integer = true, min = 1}}},
	{name = "inventory_size", type = "number", description = "Size of the inventory. Nonzero makes the item a container."},
	{name = "inventory_type", type = "string", description = "Inventory type string."},
	{name = "loot_categories", type = "list", list = {type = "string"}, description = "List of item categories this container can have as random loot."},
	{name = "loot_count_min", type = "number", description = "Minimum number of random loot items this container can have."},
	{name = "loot_count_max", type = "number", description = "Maximum number of random loot items this container can have."},
	{name = "mass", type = "number", default = 10, description = "Mass in the physics simulation, in kilograms."},
	{name = "mass_inventory", type = "number", default = 10, description = "Mass in the inventory, in kilograms."},
	{name = "model", type = "string", description = "Model to use for the item."},
	{name = "potion_effects", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of potion effects.", details = {keys = {spec = "ModifierSpec"}}},
	{name = "special_effects", type = "list", list = {type = "string", details = {value = {spec = "Effectspec"}}}, default = {}, description = "List of special effects to render."},
	{name = "stacking", type = "boolean", description = "True to allow the item to stack in the inventory."},
	{name = "timings", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of timings."},
	{name = "usages", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of ways how the object can be used.", details = {keys = {spec = "Actionspec"}}},
	{name = "water_friction", type = "number", default = 0.9},
	{name = "water_gravity", type = "vector", default = Vector(0,-3)}
})

--- Creates a new item specification.
-- @param clss Itemspec class.
-- @param args Arguments.
-- @return New item specification.
Itemspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets an animation by name.
-- @param self Itemspec.
-- @param name Animation name.
-- @param profile Animation profile mapping, or nil for "default".
-- @return Animation spec, or nil.
Itemspec.get_animation = function(self, name, profile)
	local try = function(self, p, a)
		local pname = self.animations[p]
		if not pname then return end
		local profile = AnimationProfileSpec:find{name = pname}
		if not profile then return end
		return profile:get_animation(a)
	end
	return profile and try(self, profile, name) or try(self, "default", name)
end

--- Gets an equipment animation by name.
-- @param self Itemspec.
-- @param name Animation name.
-- @param profile Animation profile mapping, or nil for "default".
-- @return Animation spec, or nil.
Itemspec.get_animation_equipped = function(self, name, profile)
	local try = function(self, p, a)
		local pname = self.animations_equipped[p]
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
-- @param variant Variant number, or nil.
-- @return Table of animation playback arguments.
Itemspec.get_animation_arguments = function(self, name, profile, variant)
	local args = {animation = name, fade_in = 0.3, fade_out = 0.3, time = time}
	local anim = self:get_animation(name, profile)
	if anim then
		for k,v in pairs(anim:get_arguments(variant)) do args[k] = v end
	end
	return args
end

--- Gets equipment animation playback arguments by name.
-- @param self Actor spec.
-- @param name Animation name.
-- @param profile Animation profile mapping, or nil for "default".
-- @param variant Variant number, or nil.
-- @return Table of animation playback arguments, or nil.
Itemspec.get_animation_arguments_equipped = function(self, name, profile, variant)
	local anim = self:get_animation_equipped(name, profile)
	if not anim then return end
	local args = {animation = name, fade_in = 0.3, fade_out = 0.3, time = time}
	if anim then
		for k,v in pairs(anim:get_arguments(variant)) do args[k] = v end
	end
	return args
end

--- Gets an effect by name.
-- @param self Itemspec.
-- @param name Effect name.
-- @param profile Effect profile name. Nil for "default".
-- @return Effect spec, or nil.
Itemspec.get_effect = function(self, name, profile)
	local try = function(self, p, e)
		local pname = self.effects[p]
		if not pname then return end
		local profile = EffectProfileSpec:find_by_name(pname)
		if not profile then return end
		return profile:get_effect(e)
	end
	return profile and try(self, profile, name) or try(self, "default", name)
end

--- Finds the equipment models of the item for the given race.
-- @param self Itemspec.
-- @param name Name of the equipment class matching the race.
-- @param lod True for low level of detail.
-- @return Table of equipment models or nil.
Itemspec.get_equipment_models = function(self, name, lod)
	-- Choose the level of detail.
	-- If the requested level doesn't exist, fall back to the other one.
	local models = nil
	if lod then models = self.equipment_models_lod end
	if not models then models = self.equipment_models end
	if not models then models = self.equipment_models_lod end
	-- Find the equipment models for the race.
	return models and models[name]
end

--- Finds the equipment textures of the item for the given race.
-- @param self Itemspec.
-- @param name Name of the equipment class matching the race.
-- @param lod True for low level of detail.
-- @return Table of equipment models or nil.
Itemspec.get_equipment_textures = function(self, name, lod)
	-- Choose the level of detail.
	-- If the requested level doesn't exist, fall back to the other one.
	local textures = nil
	if lod then textures = self.equipment_textures_lod end
	if not textures then textures = self.equipment_textures end
	if not textures then textures = self.equipment_textures_lod end
	-- Find the equipment textures for the race.
	return textures and textures[name]
end

--- Finds the special effects of the item.
-- @param self Item spec.
-- @return List of effect specs, or nil.
Itemspec.get_special_effects = function(self)
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

--- Gets the base trading value of the item.
-- @param self Itemspec.
-- @return Number.
Itemspec.get_trading_value = function(self)
	-- Return if cached.
	if self.value then return self.value end
	-- Calculate the value.
	local value = 0.2
	if self.armor_class then
		value = value + 50 * self.armor_class
	end
	if self.modifiers then
		for k,v in pairs(self.modifiers) do
			value = value + math.max(0, math.abs(v) - 3)
		end
	end
	if self.modifiers_bonus then
		for k in pairs(self.modifiers_bonus) do
			value = value + 100
		end
	end
	local craft = CraftingRecipeSpec:find_by_name(self.name)
	if craft then
		local req = Main.crafting_utils:get_requiring_items(craft)
		if #req > 0 then
			self.value = value
			local awg = 0
			for k,v in pairs(req) do
				local spec = Itemspec:find_by_name(v)
				if spec then awg = awg + spec:get_trading_value() end
			end
			awg = awg / #req
			value = value + 0.05 * awg
		end
	end
	-- Cache the value.
	self.value = value
	return value
end

--- Gets the use actions applicable to the item.
-- @param self Item spec.
-- @return List of actions specs.
Itemspec.get_use_actions = function(self)
	local res = {}
	for k,v in pairs(self.usages) do
		local a = Actionspec:find_by_name(k)
		if a then table.insert(res, a) end
	end
	return res
end

return Itemspec
