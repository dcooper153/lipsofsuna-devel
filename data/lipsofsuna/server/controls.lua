local spawn_player = function(object, client, spawnpoint)
	-- Add to the map.
	Player.clients[client] = object
	local home = object:get_spawn_point()
	if not home or spawnpoint then
		home = object:set_spawn_point(spawnpoint)
		object:teleport{position = home}
	end
	object.realized = true
	object:set_client(client)
	-- Transmit the home marker.
	object:send(Packet(packets.MARKER_ADD, "string", "home",
		"float", home.x, "float", home.y, "float", home.z))
	-- Transmit unlocked map markers.
	for k,m in pairs(Marker.dict_name) do
		if m.unlocked then
			object:send(Packet(packets.MARKER_ADD,
				"string", m.name,
				"float", m.position.x,
				"float", m.position.y,
				"float", m.position.z))
		end
	end
	-- Transmit other unlocks.
	local packet1 = Packet(packets.UNLOCK)
	Unlocks:write_packet(packet1)
	object:send(packet1)
	-- Transmit skills.
	object:update_skills()
	-- Transmit active and completed quests.
	for k,q in pairs(Quest.dict_name) do
		q:send{client = object}
		q:send_marker{client = object}
	end
	-- Transmit static objects.
	local packet = Packet(packets.CREATE_STATIC_OBJECTS)
	for k,v in pairs(Staticobject.dict_id) do
		packet:write(
			"uint32", v.id,
			"string", v.spec.name,
			"float", v.position.x,
			"float", v.position.y,
			"float", v.position.z,
			"float", v.rotation.x,
			"float", v.rotation.y,
			"float", v.rotation.z,
			"float", v.rotation.w)
	end
	object:send(packet)
	-- Transmit dialog states of static objects.
	for k,v in pairs(Dialog.dict_id) do
		if v.object and v.object.static and v.event then
			object:vision_cb(v.event)
		end
	end
end

Protocol:add_handler{type = "CHARACTER_CREATE", func = function(args)
	-- Make sure the client has been authenticated.
	local account = Account.dict_client[args.client]
	if not account then return end
	-- Make sure not created already.
	local player = Player:find{client = args.client}
	if player then return end
	-- Get character flags.
	local ok,na,ra,bs,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,head,eye,eyer,eyeg,eyeb,
	f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,
	hair,hairr,hairg,hairb,skin,skinr,sking,skinb,spawnpoint = args.packet:read(
		"string", "string",
		-- Body style.
		"uint8",
		"uint8", "uint8", "uint8", "uint8", "uint8",
		"uint8", "uint8", "uint8", "uint8", "uint8",
		-- Head style.
		"string",
		-- Eye style.
		"string", "uint8", "uint8", "uint8",
		-- Face style.
		"uint8", "uint8", "uint8", "uint8", "uint8",
		"uint8", "uint8", "uint8", "uint8", "uint8",
		"uint8", "uint8", "uint8", "uint8", "uint8",
		-- Hair style.
		"string", "uint8", "uint8", "uint8",
		-- Skin style.
		"string", "uint8", "uint8", "uint8",
		-- Spawnpoint.
		"string")
	if not ok then return end
	-- TODO: Input validation.
	local spec = Actorspec:find{name = ra .. "-player"}
	if not spec then return end
	-- Create the character.
	local o = Player{
		account = account,
		body_scale = bs,
		body_style = {b1,b2,b3,b4,b5,b6,b7,b8,b9,b10},
		eye_style = {eye, eyer, eyeg, eyeb},
		face_style = {f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15},
		hair_style = {hair, hairr, hairg, hairb},
		head_style = head,
		name = (na ~= "" and na or "Player"),
		random = true,
		skin_style = {skin, skinr, sking, skinb},
		spec = spec}
	-- Add to the map.
	Network:send{client = args.client, packet = Packet(packets.CHARACTER_ACCEPT)}
	spawn_player(o, args.client, spawnpoint)
	Serialize:save_object(o)
end}

Protocol:add_handler{type = "CLIENT_AUTHENTICATE", func = function(args)
	-- Make sure not authenticated already.
	local account = Account.dict_client[args.client]
	if account then return end
	-- Read credentials.
	local ok,login,pass = args.packet:read("string", "string")
	if not ok then
		Network:send{client = args.client, packet = Packet(packets.AUTHENTICATE_REJECT, "string", "Protocol mismatch.")}
		Network:disconnect(args.client)
		return
	end
	-- Make sure not logging in twice.
	account = Account.dict_name[login]
	if account then
		Network:send{client = args.client, packet = Packet(packets.AUTHENTICATE_REJECT, "string", "The account is already in use.")}
		return
	end
	-- Load or create the account.
	-- The password is also checked in case of an existing account. If the
	-- check fails, Account() returns nil and we disconnect the client.
	account = Account(login, pass)
	if not account then
		Network:send{client = args.client, packet = Packet(packets.AUTHENTICATE_REJECT, "string", "Invalid account name or password.")}
		Network:disconnect(args.client)
		return
	end
	account.client = args.client
	Account.dict_client[args.client] = account
	Log:format("Client login from %q using account %q.", Network:get_client_address(args.client) or "???", login)
	-- Create existing characters.
	local object = Serialize:load_player_object(account)
	if object then
		Network:send{client = args.client, packet = Packet(packets.CHARACTER_ACCEPT)}
		object.account = account
		spawn_player(object, args.client)
	end
	-- Check for permissions.
	-- Check if the account has admin rights in the config file.
	-- Grant admin rights to the first client if started with --admin.
	local admin = (Config.inst.admins[login] == true)
	if Settings.admin then
		Settings.admin = nil
		Config.inst.admins[login] = true
		admin = true
	end
	-- Inform about admin privileges.
	Network:send{client = args.client, packet = Packet(packets.ADMIN_PRIVILEGE, "bool", admin)}
	-- Enter the character creation mode.
	if not object then
		Network:send{client = args.client, packet = Packet(packets.CHARACTER_CREATE)}
	end
end}

Protocol:add_handler{type = "FEAT", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Players are able to create custom feats on client side. When
	-- the feat is performed, the client sends us all the information
	-- on the feat. We then reconstruct and perform the feat.
	local ok,anim,e1,v1,e2,v2,e3,v3,on = args.packet:read("string",
		"string", "float", "string", "float", "string", "float", "bool")
	if not ok then return end
	if anim == "" or not Feattypespec:find{name = anim} then anim = nil end
	if e1 == "" or not Feateffectspec:find{name = e1} then e1 = nil end
	if e2 == "" or not Feateffectspec:find{name = e2} then e2 = nil end
	if e3 == "" or not Feateffectspec:find{name = e3} then e3 = nil end
	local feat = Feat{animation = anim, effects = {
		e1 and {e1, 1}, e2 and {e2, 1}, e3 and {e3, 1}}}
	-- Inform the client about failures.
	local ok,msg = feat:usable{user = player}
	if on and not ok then
		player:send(msg)
	end
	-- Perform the feat.
	feat:perform{stop = not on, user = player}
end}

Protocol:add_handler{type = "INVENTORY_CLOSED", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory ID.
	local ok,id = args.packet:read("uint32")
	if not ok then return end
	if id == player.id then return end
	-- Find the inventory.
	local obj = Object:find{id = id}
	if not obj then return end
	-- Unsubscribe.
	obj.inventory:unsubscribe(player)
end}

Protocol:add_handler{type = "MOVE_ITEM", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the source and destination types.
	local ok,src,dst,count = args.packet:read("uint8", "uint8", "uint32")
	if not ok then return end
	if src == moveitem.WORLD and dst == moveitem.WORLD then return end
	-- Read the detailed source information.
	local srcid = nil
	local srcslot = nil
	if src == moveitem.EQUIPMENT then
		ok,srcid,srcslot = args.packet:resume("uint32", "string")
		if not ok then return end
	elseif src == moveitem.INVENTORY then
		ok,srcid,srcslot = args.packet:resume("uint32", "uint8")
		if not ok then return end
	elseif src == moveitem.WORLD then
		ok,srcid = args.packet:resume("uint32")
		if not ok then return end
	else return end
	-- Read the detailed destination information.
	local dstid = nil
	local dstslot = nil
	if dst == moveitem.EQUIPMENT then
		ok,dstid,dstslot = args.packet:resume("uint32", "string")
		if not ok then return end
	elseif dst == moveitem.INVENTORY then
		ok,dstid,dstslot = args.packet:resume("uint32", "uint8")
		if not ok then return end
	elseif dst ~= moveitem.WORLD then return end
	-- Perform the item move.
	if src == moveitem.WORLD then
		player:pick_up(srcid, dstid, dstslot)
	elseif dst == moveitem.WORLD then
		player:animate("drop")
		Timer{delay = player.spec.timing_drop * 0.02, func = function(timer)
			Actions:move_from_inv_to_world(player, srcid, srcslot, count)
			timer:disable()
		end}
	else
		return Actions:move_from_inv_to_inv(player, srcid, srcslot, dstid, dstslot, count)
	end
end}

Protocol:add_handler{type = "PLAYER_ATTACK", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the packet.
	local ok,v = args.packet:read("bool")
	if not ok then return end
	-- Handle auto-attack.
	if v then
		player.auto_attack = true
	else
		player.auto_attack = nil
	end
	-- Handle attacks.
	if v and player.attack_charge then return end
	if not v and not player.attack_charge then return end
	if v then
		player:attack_charge_start()
	else
		player:attack_charge_end(args)
	end
end}

Protocol:add_handler{type = "PLAYER_BLOCK", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	local ok,v = args.packet:read("bool")
	if not ok then return end
	player:set_block(v)
end}

Protocol:add_handler{type = "PLAYER_CLIMB", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	player:climb()
end}

Protocol:add_handler{type = "PLAYER_DIALOG", func = function(args)
	-- Get the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the object ID.
	local ok,id = args.packet:read("uint32")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	if not player:can_reach_object(object) then return end
	-- Execute the dialog of the object.
	if object.dialog then return end
	local dialog = Dialog{object = object, user = player}
	if not dialog then return end
	object.dialog = dialog
	object.dialog:execute()
end}

Protocol:add_handler{type = "PLAYER_DROP", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory index and drop count.
	local ok,index,count = args.packet:read("uint32", "uint32")
	if not ok then return end
	-- Get the item.
	local object = player.inventory:get_object_by_index(index)
	if not object then return end
	-- Split the dropped stack.
	count = math.min(object.count, count)
	local split = object:split(count)
	-- Drop the item.
	-- TODO: Better positioning.
	split.position = player.position + player.rotation * Vector(0,0,-1) + Vector(0,1)
	split.velocity = Vector()
	split.rotation = Quaternion()
	split.realized = true
end}

Protocol:add_handler{type = "PLAYER_EQUIP", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory index and slot.
	local ok,index,slot = args.packet:read("uint32", "string")
	if not ok then return end
	-- Get the item and validate the equip.
	local object = player.inventory:get_object_by_index(index)
	if not object then return end
	if object.spec.equipment_slot ~= slot then return end
	-- Equip the item in the slot.
	player.inventory:equip_index(index, slot)
end}

Protocol:add_handler{type = "PLAYER_JUMP", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	local ok,on = args.packet:read("bool")
	if not ok then return end
	if on then
		player:jump()
	else
		player:jump_stop()
	end
end}

Protocol:add_handler{type = "PLAYER_LOOT_INVENTORY", func = function(args)
	-- Get the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory index.
	local ok,inv,index = args.packet:read("uint32", "uint32")
	if not ok then return end
	-- Get the object.
	local parent = Object:find{id = inv}
	if not parent.inventory:is_subscribed(player) then return end
	if not player:can_reach_object(parent) then return end
	local object = parent.inventory:get_object_by_index(index)
	if not object then return end
	-- Use the object.
	object:loot(player)
end}

Protocol:add_handler{type = "PLAYER_LOOT_WORLD", func = function(args)
	-- Get the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the object ID.
	local ok,id = args.packet:read("uint32")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	if not player:can_reach_object(object) then return end
	-- Use the object.
	object:loot(player)
end}

Protocol:add_handler{type = "PLAYER_MOVE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	local ok,v = args.packet:read("int8")
	if not ok then return end
	if v > 0 then
		player:set_movement(1)
	elseif v < 0 then
		player:set_movement(-1)
	else
		player:set_movement(0)
	end
end}

Protocol:add_handler{type = "PLAYER_MOVE_ITEM", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory id and indices.
	local ok,id,src,dst = args.packet:read("uint32", "uint32", "uint32")
	if not ok then return end
	if src == dst then return end
	-- Get the modified inventory.
	local target = Object:find{id = id}
	if not target then return end
	if not target.inventory:is_subscribed(player) then return end
	if not player:can_reach_object(target) then return end
	-- Get the moved item and validate the move.
	local item = target.inventory:get_object_by_index(src)
	if not item then return end
	if dst == 0 or dst > target.inventory.size then return end
	-- Move, merge or swap the items.
	local swapped = target.inventory:get_object_by_index(dst)
	if swapped then
		local slot1 = target.inventory:get_slot_by_index(src)
		local slot2 = target.inventory:get_slot_by_index(dst)
		if slot1 or slot2 or not target.inventory:merge_object_to_index(dst, item) then
			target.inventory:set_object(dst, item)
			target.inventory:set_object(src, swapped)
			if slot1 then target.inventory:equip_index(dst, slot1) end
			if slot2 then target.inventory:equip_index(src, slot2) end
		end
	else
		local slot1 = target.inventory:get_slot_by_index(src)
		item:detach()
		target.inventory:set_object(dst, item)
		if slot1 then target.inventory:equip_index(dst, slot1) end
	end
end}

Protocol:add_handler{type = "PLAYER_PICKPOCKET", func = function(args)
	-- Get the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the object ID.
	local ok,id = args.packet:read("uint32")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	if not player:can_reach_object(object) then return end
	-- Use the object.
	-- FIXME: Should use a different system.
	object:loot(player)
end}

Protocol:add_handler{type = "PLAYER_PICKUP", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the object ID.
	local ok,id = args.packet:read("uint32")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	if object.spec.type ~= "item" then return end
	if not player:can_reach_object(object) then return end
	-- Pick up the object.
	player.inventory:merge_object(object)
	player:animate("pick up")
end}

Protocol:add_handler{type = "PLAYER_RESPAWN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then return end
	player:respawn()
end}

Protocol:add_handler{type = "PLAYER_RUN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	local ok,on = args.packet:read("bool")
	if not ok then return end
	player.running = on
	player:calculate_speed()
end}

Protocol:add_handler{type = "PLAYER_SKILLS", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read packet data.
	local enabled = {}
	while true do
		local ok,s = args.packet:resume("string")
		if not ok then break end
		enabled[s] = true
	end
	-- Validate the input.
	for k,v in pairs(enabled) do
		local skill = Skillspec:find{name = k}
		if not skill or not Unlocks:get("skill", k) then
			enabled[k] = nil
		end
	end
	-- Enable and disable skills.
	player.skills:clear()
	for k in pairs(enabled) do
		player.skills:add_without_requirements(k)
	end
	player.skills:remove_invalid()
	-- Recalculate player attributes.
	player:update_skills()
end}

Protocol:add_handler{type = "PLAYER_SPLIT_ITEM", func = function(args)
	-- Get the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory index.
	local ok,inv,index,count = args.packet:read("uint32", "uint32", "uint32")
	if not ok then return end
	-- Get the object.
	local parent = Object:find{id = inv}
	if not parent.inventory:is_subscribed(player) then return end
	if not player:can_reach_object(parent) then return end
	local object = parent.inventory:get_object_by_index(index)
	if not object then return end
	-- Validate the split.
	if count == 0 then return end
	if not object.spec.stacking then return end
	if not object.count or object.count < 2 then return end
	count = math.max(1, count)
	count = math.min(count, object.count - 1)
	-- Split the stack.
	local dst = parent.inventory:get_empty_index()
	if not dst then
		player:send("No room for the split item")
		return
	end
	local split = object:split(count)
	parent.inventory:update_index(index)
	parent.inventory:set_object(dst, split)
end}

Protocol:add_handler{type = "PLAYER_STORE", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory id and index.
	local ok,id,index = args.packet:read("uint32", "uint32")
	if not ok then return end
	-- Get the item and validate the store.
	local target = Object:find{id = id}
	if not target then return end
	if not target.inventory:is_subscribed(player) then return end
	if not player:can_reach_object(target) then return end
	local object = player.inventory:get_object_by_index(index)
	if not object then return end
	-- Store the item.
	if not target.inventory:merge_object(object) then
		player:send("Could not store the item.")
	end
end}

Protocol:add_handler{type = "PLAYER_STRAFE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	local ok,val = args.packet:read("int8")
	if not ok then return end
	player:set_strafing(val / 127)
end}

Protocol:add_handler{type = "PLAYER_TAKE", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory id and index.
	local ok,id,index = args.packet:read("uint32", "uint32")
	if not ok then return end
	-- Get the item and validate the take.
	local parent = Object:find{id = id}
	if not parent then return end
	if not parent.inventory:is_subscribed(player) then return end
	if not player:can_reach_object(parent) then return end
	local object = parent.inventory:get_object_by_index(index)
	if not object then return end
	-- Take the item.
	player.inventory:merge_object(object)
end}

Protocol:add_handler{type = "PLAYER_TURN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	local ok,x,y,z,w = args.packet:read("float", "float", "float", "float")
	if not ok then return end
	local e = Quaternion(x, y, z, w).euler
	e[3] = math.min(player.spec.tilt_limit, e[3])
	e[3] = math.max(-player.spec.tilt_limit, e[3])
	player.tilt = Quaternion{euler = {0, 0, e[3]}}
	player.rotation = Quaternion{euler = {e[1], e[2], 0}}
	Vision:event{type = "object-moved", object = o}
end}

Protocol:add_handler{type = "PLAYER_UNEQUIP", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory index.
	local ok,index = args.packet:read("uint32")
	if not ok then return end
	-- Get the item and validate the equip.
	local object = player.inventory:get_object_by_index(index)
	if not object then return end
	-- Equip the item in the slot.
	player.inventory:unequip_index(index)
end}

Protocol:add_handler{type = "PLAYER_USE_INVENTORY", func = function(args)
	-- Get the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the inventory index.
	local ok,inv,index,action = args.packet:read("uint32", "uint32", "string")
	if not ok then return end
	-- Get the object.
	local parent = Object:find{id = inv}
	if not parent.inventory:is_subscribed(player) then return end
	if not player:can_reach_object(parent) then return end
	local object = parent.inventory:get_object_by_index(index)
	if not object then return end
	-- Validate the action.
	if not object.spec.usages[action] then return end
	action = Actionspec:find{name = action}
	if not action then return end
	if not action.func then return end
	-- Perform the action.
	action.func(object, player)
end}

Protocol:add_handler{type = "PLAYER_USE_WORLD", func = function(args)
	-- Get the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the object ID.
	local ok,id,action = args.packet:read("uint32", "string")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	if not player:can_reach_object(object) then return end
	-- Validate the action.
	if not object.spec.usages[action] then return end
	action = Actionspec:find{name = action}
	if not action then return end
	if not action.func then return end
	-- Perform the action.
	action.func(object, player)
end}
