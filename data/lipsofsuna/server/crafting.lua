local oldcancraft = Crafting.can_craft 

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
	if not clss:can_craft{spec = spec, user = args.user} then return end
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

--- Checks if a specific item can be crafted by the user.
-- @param clss Crafting class.
-- @param args Arguments.<ul>
--   <li>spec: Item specification.</li>
--   <li>user: Object.</li></ul>
-- @return True if can craft.
Crafting.can_craft = function(clss, args)
	if not args.user then return true end
	local inv = args.user.inventory
	if not inv then return end
	local get_item = function(name)
		return args.user.inventory:count_objects_by_name(name)
	end
	local get_skill = function(name)
		return args.user.skills:get_value{skill = name}
	end
	return oldcancraft(clss, {get_item = get_item, get_skill = get_skill, spec = args.spec})
end

------------------------------------------------------------------------------

--- Handles a crafting packet sent by a client.
Protocol:add_handler{type = "CRAFTING", func = function(args)
	local ok,id,name = args.packet:read("uint32", "string")
	if not ok then return end
	-- Try to craft the requested item.
	local player = Player:find{client = args.client}
	local o = Crafting:craft{name = name, user = player}
	player.inventory:merge_or_drop_object(o)
end}
