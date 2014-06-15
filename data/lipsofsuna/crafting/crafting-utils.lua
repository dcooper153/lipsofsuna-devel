--- Crafting utils.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module crafting.crafting_utils
-- @alias CraftingUtils

local Class = require("system/class")

--- Crafting utils.
-- @type CraftingUtils
local CraftingUtils = Class("CraftingUtils")

--- Creates new crafting utils.
-- @param clss CraftingUtils class.
-- @return CraftingUtils.
CraftingUtils.new = function(clss)
	local self = Class.new(clss)
	return self
end

--- Checks if a specific item can be crafted by the user.
-- @param self CraftingUtils.
-- @param spec CraftingRecipeSpec.
-- @param user User object, or nil to get all craftable items.
-- @param mode Crafting mode, or nil for default.
-- @return True if can craft.
CraftingUtils.can_craft = function(self, spec, user, mode)
	-- Check for the mode.
	--
	-- Generally, items that do not require a crafting device cannot be
	-- crafted while using a crafting device. This is the case so that,
	-- for example, easy foods do not appear in the blacksmithing list.
	if not spec then return end
	if not spec.modes[mode or "default"] then return end
	-- Check for required materials.
	for name,req in pairs(spec.materials) do
		if user then
			if not user.inventory then return end
			local cnt = user.inventory:count_objects_by_name(name)
			if not cnt or cnt < req then return end
		end
	end
	return true
end

--- Crafts an item and returns it.
-- @param self CraftingUtils.
-- @param user Actor performing the crafting.
-- @param name Item name.
-- @param mode Crafting mode, or nil for default.
-- @return Object or nil.
CraftingUtils.craft = function(self, user, name, mode)
	-- Get the specs.
	local craftspec = Main.specs:find_by_name("CraftingRecipeSpec", name)
	if not craftspec then return end
	local itemspec = Main.specs:find_by_name("ItemSpec", name)
	if not itemspec then return end
	-- Check for requirements.
	--
	-- This allows crafting items without a device even if the player has
	-- a device open in server side. In client side, the device might have
	-- been closed, but we do not want to track that on the server.
	if not self:can_craft(craftspec, user, mode) and
	   not self:can_craft(craftspec, user, "default") then
		return
	end
	-- Consume materials.
	for name1,req in pairs(craftspec.materials) do
		user.inventory:subtract_objects_by_name(name1, req)
	end
	-- Play the crafting effect.
	Main.vision:object_effect(user, craftspec.effect)
	-- Create item.
	local item = user.manager:create_object("Item")
	item:set_spec(itemspec)
	item:set_count(craftspec.count)
	return item
end

--- Disenchants an item.
-- @param self CraftingUtils.
-- @param item Item.
CraftingUtils.disenchant_item = function(self, item)
	item.spell_action = nil
	item.spell_modifiers = nil
end

--- Enchants an item.
-- @param self CraftingUtils.
-- @param item Item.
-- @param action Action name.
-- @param Dictionary of modifiers and their values.
CraftingUtils.enchant_item = function(self, item, action, modifiers)
	item.spell_action = action
	item.spell_modifiers = {}
	for k,v in pairs(modifiers) do
		item.spell_modifiers[k] = v
	end
end

--- Checks if the item is enchanted.
-- @param self CraftingUtils.
-- @param item Item.
-- @return True if enchanted. False otherwise.
CraftingUtils.is_item_enchanted = function(self, item)
	return item.spell_action ~= nil
end

--- Gets the names of all craftable items.
-- @param self CraftingUtils.
-- @param user User object, or nil to get all craftable items.
-- @param mode Crafting mode, or nil for default.
-- @return Table of item names.
CraftingUtils.get_craftable = function(self, user, mode)
	local items = {}
	for name,spec in pairs(Main.specs:get_spec_names("CraftingRecipeSpec")) do
		if self:can_craft(spec, user, mode) then
			table.insert(items, name)
		end
	end
	return items
end

--- Gets the names of items that require the required itemspec for crafting.
-- @param self CraftingUtils.
-- @param spec ItemSpec.
-- @return Table of item names.
CraftingUtils.get_requiring_items = function(self, spec)
	local items = {}
	for k,v in pairs(Main.specs:get_spec_names("CraftingRecipeSpec")) do
		if v.materials[spec.name] then
			table.insert(items, k)
		end
	end
	return items
end

return CraftingUtils
