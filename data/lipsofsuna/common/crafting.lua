--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module common.crafting
-- @alias Crafting

local Class = require("system/class")
local Item = require("core/objects/item")

--- TODO:doc
-- @type Crafting
Crafting = Class("Crafting")

--- Checks if a specific item can be crafted by the user.
-- @param clss Crafting class.
-- @param spec Crafting recipe specification.
-- @param user User object, or nil to get all craftable items.
-- @param mode Crafting mode, or nil for default.
-- @return True if can craft.
Crafting.can_craft = function(clss, spec, user, mode)
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
-- @param clss Crafting class.
-- @param user Actor performing the crafting.
-- @param name Item name.
-- @param mode Crafting mode, or nil for default.
-- @return Object or nil.
Crafting.craft = function(clss, user, name, mode)
	-- Get the specs.
	local craftspec = CraftingRecipeSpec:find{name = name}
	if not craftspec then return end
	local itemspec = Itemspec:find{name = name}
	if not itemspec then return end
	-- Check for requirements.
	--
	-- This allows crafting items without a device even if the player has
	-- a device open in server side. In client side, the device might have
	-- been closed, but we do not want to track that on the server.
	if not clss:can_craft(craftspec, user, mode) and
	   not clss:can_craft(craftspec, user, "default") then
		return
	end
	-- Consume materials.
	for name1,req in pairs(craftspec.materials) do
		user.inventory:subtract_objects_by_name(name1, req)
	end
	-- Play the crafting effect.
	Server:object_effect(user, craftspec.effect)
	-- Create item.
	return Item{count = craftspec.count, spec = itemspec}
end

--- Gets the names of all craftable items.
-- @param clss Crafting class.
-- @param user User object, or nil to get all craftable items.
-- @param mode Crafting mode, or nil for default.
-- @return Table of item names.
Crafting.get_craftable = function(clss, user, mode)
	local items = {}
	for name,spec in pairs(CraftingRecipeSpec.dict_name) do
		if clss:can_craft(spec, user, mode) then
			table.insert(items, name)
		end
	end
	return items
end

--- Gets the names of items that require the required itemspec for crafting.
-- @param clss Crafting class.
-- @param spec Crafting recipe specification.
-- @return Table of item names.
Crafting.get_requiring_items = function(clss, spec)
	local items = {}
	for k,v in pairs(CraftingRecipeSpec.dict_name) do
		if v.materials[spec.name] then
			table.insert(items, k)
		end
	end
	return items
end


