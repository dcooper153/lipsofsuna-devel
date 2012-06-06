Crafting = Class()

--- Checks if a specific item can be crafted by the user.
-- @param clss Crafting class.
-- @param spec Item specification.
-- @param user User object, or nil to get all craftable items.
-- @return True if can craft.
Crafting.can_craft = function(clss, spec, user)
	-- Check for the crafting enable.
	if not spec or not spec.crafting_enabled then return end
	-- Check for required materials.
	for name,req in pairs(spec.crafting_materials) do
		if user then
			if not user.inventory then return end
			local cnt = user.inventory:count_objects_by_name(name)
			if not cnt or cnt < req then return end
		end
	end
	return true
end

--- Gets the names of all craftable items.
-- @param clss Crafting class.
-- @param user User object, or nil to get all craftable items.
-- @return Table of item names.
Crafting.get_craftable = function(clss, user)
	local items = {}
	for name,spec in pairs(Itemspec.dict_name) do
		if clss:can_craft(spec, user) then
			table.insert(items, name)
		end
	end
	return items
end

--- Gets the names of items that require the required itemspec for crafting.
-- @param clss Crafting class.
-- @param spec Itemspec.
-- @return Table of item names.
Crafting.get_requiring_items = function(clss, spec)
	local items = {}
	for k,v in pairs(Itemspec.dict_name) do
		if v.crafting_materials[spec.name] then
			table.insert(items, k)
		end
	end
	return items
end
