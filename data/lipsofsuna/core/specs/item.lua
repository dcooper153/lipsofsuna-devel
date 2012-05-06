require(Mod.path .. "spec")
require "common/string"

Itemspec = Class(Spec)
Itemspec.type = "item"
Itemspec.dict_id = {}
Itemspec.dict_cat = {}
Itemspec.dict_name = {}
Itemspec.introspect = Introspect{
	name = "Itemspec",
	fields = {
		{name = "name", type = "string", description = "Name of the spec."},
		{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
		{name = "ammo_type", type = "string", description = "Name of the ammunition item type.", details = {spec = "Itemspec"}},
		{name = "animation_looted", type = "string", description = "Animation played when the item is looted."},
		{name = "animation_looting", type = "string", description = "Animation played when the item is being open during looting."},
		{name = "animation_attack", type = "string", description = "Attack animation name for actors wielding the item."},
		{name = "animation_charge", type = "string", description = "Charge animation name for actors wielding the item."},
		{name = "animation_hold", type = "string", description = "Hold animation name for actors wielding the item."},
		{name = "animations", type = "dict", dict = {type = "string"}, default = {}, description = "Dictionary of animations.", details = {values = {spec = "Animationspec"}}},
		{name = "armor_class", type = "number", default = 0, description = "How much protection the item offers when equipped."},
		{name = "book_text", type = "string", description = "Content of player readable items."},
		{name = "collision_group", type = "number", default = 0x0002, description = "Collision group."},
		{name = "construct_tile", type = "string", description = "Tile type to construct when used."},
		{name = "construct_tile_count", type = "number", description = "Material count required by tile construction.", details = {integer = true, min = 1}},
		{name = "crafting_count", type = "number", default = 1, description = "Number of items crafted per use.", details = {integer = true, min = 1}},
		{name = "crafting_materials", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of crafting materials.", details = {keys = {spec = "Itemspec"}, values = {integer = true, min = 1}}},
		{name = "damage_mining", type = "number", description = "Damage the item takes from mining."},
		{name = "description", type = "string", description = "Description of the item."},
		{name = "destroy_actions", type = "list", list = {type = "string"}, default = {}, description = "List of actions to perform when the item is destroyed."},
		{name = "destroy_timer", type = "number", description = "Time in seconds after which to destruct when thrown."},
		{name = "dialog", type = "string", description = "Dialog name.", details = {spec = "Dialogspec"}},
		{name = "effect_attack", type = "string", description = "Name of the effect to play when the item is used for attacking."},
		{name = "effect_attack_speedline", type = "boolean", description = "True to enable the speed line effect for attacks."},
		{name = "effect_craft", type = "string", description = "Name of the effect to play when the item is crafted."},
		{name = "effect_equip", type = "string", description = "Name of the effect to play when the item is equiped."},
		{name = "effect_unequip", type = "string", description = "Name of the effect to play when the item is unequiped."},
		{name = "effect_use", type = "string", description = "Name of the effect to play when the item is used."},
		{name = "equipment_models", type = "dict", dict = {type = "dict", dict = {type = "string"}}, description = "Dictionary of equipment models."},
		{name = "equipment_slot", type = "string", description = "Equipment slot into which the item can be placed."},
		{name = "equipment_slots_reserved", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of equipment slots that the item reserves in addition to the main slot."},
		{name = "gravity", type = "vector", default = Vector(0,-15), description = "Gravity vector."},
		{name = "gravity_projectile", type = "vector", default = Vector(0,-15), description = "Gravity vector for projectile mode."},
		{name = "health", type = "number", description = "Number of hit points the item has."},
		{name = "influences", type = "dict", dict = {type = "number"}, description = "Dictionary of ways how attacks with the weapon influence the target."},
		{name = "influences_bonus", type = "list", list = {type = "string"}, description = "List of skill-derived attribute names that affect influences."},
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
		{name = "potion_effects", type = "dict", dict = {type = "number"}, default = {}, description = "Dictionary of potion effects.", details = {keys = {spec = "Feateffectspec"}}},
		{name = "special_effects", type = "list", list = {type = "string", details = {value = {spec = "Effectspec"}}}, default = {}, description = "List of special effects to render."},
		{name = "stacking", type = "boolean", description = "True to allow the item to stack in the inventory."},
		{name = "usages", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of ways how the object can be used.", details = {keys = {spec = "Actionspec"}}},
		{name = "water_friction", type = "number", default = 0.9},
		{name = "water_gravity", type = "vector", default = Vector(0,-3)}
	}}

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
-- @return Animation spec, or nil.
Itemspec.get_animation = function(self, name)
	local n = self.animations[name]
	if not n then return end
	return Animationspec:find{name = n}
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
	if self.influences then
		for k,v in pairs(self.influences) do
			value = value + math.max(0, math.abs(v) - 3)
		end
	end
	if self.influences_bonus then
		for k in pairs(self.influences_bonus) do
			value = value + 100
		end
	end
	local req = Crafting:get_requiring_items(self)
	if #req > 0 then
		self.value = value
		local awg = 0
		for k,v in pairs(req) do
			local spec = Itemspec:find{name = v}
			if spec then awg = awg + spec:get_trading_value() end
		end
		awg = awg / #req
		value = value + 0.05 * awg
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
		local a = Actionspec:find{name = k}
		if a then table.insert(res, a) end
	end
	return res
end

Itemspec:add_getters{
	crafting_enabled = function(self)
		for k,v in pairs(self.crafting_materials) do
			return true
		end
	end}
