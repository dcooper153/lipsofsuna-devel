--- Combat utils.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module arena.combat_utils
-- @alias CombatUtils

local Class = require("system/class")

--- Combat utils.
-- @type CombatUtils
local CombatUtils = Class("CombatUtils")

--- Creates new combat utils.
-- @param clss CombatUtils class.
-- @return CombatUtils.
CombatUtils.new = function(clss)
	local self = Class.new(clss)
	return self
end

--- Calculates the armor class of the actor.
-- @param self CombatUtils.
-- @param actor Actor.
-- @return Number.
CombatUtils.get_actor_armor_class = function(self, actor)
	-- Get the inventory.
	local inv = actor.inventory
	if not inv then return 0 end
	-- Get the actor spec.
	local spec = actor:get_spec()
	if not spec then return 0 end
	-- Add the armor class from each equipped item.
	local value = 0
	for slot in pairs(spec.equipment_slots) do
		local item = inv:get_object_by_slot(slot)
		if item and item.spec and item.spec.armor_class then
			value = value + item.spec.armor_class
		end
	end
	return value
end

--- Calculates the armor class of the item.
-- @param self CombatUtils.
-- @param item Item.
-- @return Number.
CombatUtils.get_item_armor_class = function(self, item)
	if not item.spec then return 0 end
	if not item.spec.armor_class then return 0 end
	return item.spec.armor_class
end

--- Gets the weapon damage types of the item.
-- @param self CombatUtils.
-- @param actor Actor.
-- @param self Item.
-- @return Array of influences.
CombatUtils.get_item_attack_influences = function(self, actor, item)
	-- Get the item spec.
	local spec = item:get_spec()
	if not spec then return {} end
	if not spec.influences then return {} end
	-- Calculate the damage multiplier.
	local mult = 1
	if actor.skills then
		mult = actor.skills:calculate_damage_multiplier_for_item(item)
	end
	-- Calculate influences.
	local influences = {}
	for k,v in pairs(spec.influences) do
		influences[k] = mult * v
	end
	return influences
end

--- Calculates the usefulness rating of the item for the given actor.
-- @param self CombatUtils.
-- @param actor Actor.
-- @param self Item.
-- @return Number
CombatUtils.get_item_equip_value = function(self, actor, item)
	local score = 50 * self:get_item_armor_class(actor, item)
	for k,v in pairs(self:get_item_attack_influences(actor, item)) do
		if k ~= "hatchet" then
			score = score + v
		end
	end
	return score
end

return CombatUtils
