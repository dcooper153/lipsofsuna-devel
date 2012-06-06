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
	if spec.effect_craft then
		Effect:play{effect = spec.effect_craft, object = args.user}
	end
	-- Create item.
	return Item{count = spec.crafting_count, spec = spec}
end

------------------------------------------------------------------------------

--- Handles a crafting packet sent by a client.
Protocol:add_handler{type = "CRAFTING", func = function(args)
	local ok,id,name = args.packet:read("uint32", "string")
	if not ok then return end
	-- Try to craft the requested item.
	local player = Player:find{client = args.client}
	local o = Crafting:craft{name = name, user = player}
	if not o then return end
	player.inventory:merge_or_drop_object(o)
end}
