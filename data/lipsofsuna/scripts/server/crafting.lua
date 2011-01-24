Crafting = Class()

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
		args.user:subtract_items{name = name, count = req}
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
	local spec = args.spec
	if not spec or not spec.crafting_enabled then return end
	if not args.user then return true end
	-- Check for skills.
	local ski = Skills:find{owner = args.user}
	for name,req in pairs(spec.crafting_skills) do
		if not ski then return end
		local val = ski:get_value{skill = name}
		if not val or val < req then return end
	end
	-- Check for tools.
	local inv = Inventory:find{owner = args.user}
	for name in pairs(spec.crafting_tools) do
		if not inv then return end
		local obj = inv:find_object{name = name}
		if not obj then return end
	end
	-- Check for materials.
	for name,req in pairs(spec.crafting_materials) do
		if not inv then return end
		local obj = inv:find_object{name = name}
		local cnt = obj and obj:get_count() or 0
		if cnt < req then return end
	end
	return true
end

--- Gets the names of all craftable items.
-- @param clss Crafting class.
-- @param args Arguments.<ul>
--   <li>user: Object.</li></ul>
-- @return Table of item names.
Crafting.get_craftable = function(clss, args)
	local items = {}
	for name,spec in pairs(Itemspec.dict_name) do
		if clss:can_craft{name = name, spec = spec, user = args.user} then
			table.insert(items, name)
		end
	end
	return items
end

--- Sends a crafting packet to a client.
-- @param clss Crafting class.
-- @param args Arguments.<ul>
--   <li>user: Object.</li></ul>
Crafting.send = function(clss, args)
	local t = clss:get_craftable(args)
	local p = Packet(packets.CRAFTING, "uint8", 0)
	for k,v in pairs(t) do
		p:write("string", v)
	end
	args.user:send{packet = p}
end

------------------------------------------------------------------------------

--- Handles a crafting packet sent by a client.
Protocol:add_handler{type = "CRAFTING", func = function(args)
	local ok,i = args.packet:read("string")
	if not ok then return end
	local player = Player:find{client = args.client}
	local o = Crafting:craft{name = i, user = player}
	if o and not player:add_item{object = o} then
		o.position = player.position
		o.realized = true
	end
end}
