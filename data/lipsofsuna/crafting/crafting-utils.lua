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

return CraftingUtils
