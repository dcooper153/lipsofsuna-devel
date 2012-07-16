--- Crafts an item and returns it.
-- @param clss Crafting class.
-- @param args Arguments.<ul>
--   <li>name: Item name.</li>
--   <li>user: Object.</li></ul>
-- @return Object or nil.
Crafting.craft = function(clss, args)
	-- Check for requirements.
	local spec = Itemspec:find(args)
	if not spec then return end
	if not clss:can_craft(spec, args.user) then return end
	-- Consume materials.
	for name,req in pairs(spec.crafting_materials) do
		args.user.inventory:subtract_objects_by_name(name, req)
	end
	-- Play the crafting effect.
	Server:object_effect(args.user, spec.effect_craft)
	-- Create item.
	return Item{count = spec.crafting_count, spec = spec}
end
