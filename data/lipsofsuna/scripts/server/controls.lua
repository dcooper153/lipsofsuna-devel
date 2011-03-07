local spawn_player = function(object, client)
	-- Inform client.
	Network:send{client = client, packet = Packet(packets.CHARACTER_ACCEPT)}
	-- Add to the map.
	Player.clients[client] = object
	object:teleport{position = Config.spawn}
	object.realized = true
	-- Transmit active and completed quests.
	for k,q in pairs(Quest.dict_name) do
		q:send{client = object}
		q:send_marker{client = object}
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
	local ok,na,ra,s1,s2,s3,s4,s5,s6,bo,no,bu,eye,eyer,eyeg,eyeb,
	hair,hairr,hairg,hairb,skin,skinr,sking,skinb = args.packet:read(
		"string", "string",
		"uint8", "uint8", "uint8", "uint8", "uint8", "uint8",
		"float", "float", "float",
		"string", "uint8", "uint8", "uint8",
		"string", "uint8", "uint8", "uint8",
		"string", "uint8", "uint8", "uint8")
	if not ok then return end
	local spec = Species:find{name = ra .. "-player"}
	if not spec then return end
	-- TODO: Input validation.
	-- Create character. To prevent the player from falling inside the ground
	-- when spawned in a yet to be loaded region, we disable the physics of
	-- the object for a short while.
	local o = Player{
		account = account,
		body_scale = bo,
		bust_scale = bu,
		client = args.client,
		eye_style = {eye, eyer, eyeg, eyeb},
		hair_style = {hair, hairr, hairg, hairb},
		name = (na ~= "" and na or "Player"),
		nose_scale = no,
		random = true,
		skin_style = {skin, skinr, sking, skinb},
		spec = spec}
	-- Set skills.
	local names = {"dexterity", "health", "intelligence", "perception", "strength", "willpower"}
	local values = {s1, s2, s3, s4, s5, s6}
	for i = 1,#names do o:set_skill(names[i], 0) end
	for i = 1,#names do
		o:set_skill(names[i], values[i])
		local real = o.skills:get_maximum{skill = names[i]}
		o.skills:set_value{skill = names[i], value = 0.666 * real}
	end
	-- Add to the map.
	spawn_player(o, args.client)
	Serialize:save_account(account, o)
end}

Protocol:add_handler{type = "CLIENT_AUTHENTICATE", func = function(args)
	-- Make sure not authenticated already.
	local account = Account.dict_client[args.client]
	if account then return end
	-- Read credentials.
	local ok,login,pass = args.packet:read("string", "string")
	if not ok then
		Network:disconnect(args.client)
		return
	end
	-- Load or create the account.
	-- The password is also checked in case of an existing account. If the
	-- check fails, Account() returns nil and we disconnect the client.
	account = Account(login, pass)
	if not account then
		Network:disconnect(args.client)
		return
	end
	Account.dict_client[args.client] = account
	-- Create existing characters.
	if account.character then
		local func = assert(loadstring("return function()\n" .. account.character .. "\nend"))()
		if func then
			local object = func()
			if object then
				Network:send{client = args.client, packet = Packet(packets.CHARACTER_ACCEPT)}
				object.account = account
				object:set_client(args.client)
				spawn_player(object, args.client)
				return
			end
		end
	end
	-- Enter the character creation mode.
	Network:send{client = args.client, packet = Packet(packets.CHARACTER_CREATE)}
end}

Protocol:add_handler{type = "CROUCH", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,on = args.packet:read("bool")
		if ok and on then
			player:animate{animation = "crouch", channel = Animation.CHANNEL_CROUCH, weight = 0.5, permanent = true}
			player.crouch = true
		else
			player:animate{channel = Animation.CHANNEL_CROUCH}
			player.crouch = nil
		end
	end
end}

Protocol:add_handler{type = "EXAMINE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,inv,slot = args.packet:read("uint32", "uint32")
		if ok then
			local object = player:find_target(inv, slot)
			if object then
				if object.examine_cb then
					object:examine_cb(player)
				else
					player:send{packet = Packet(packets.MESSAGE, "string",
						"There's nothing special about this object.")}
				end
			end
		end
	end
end}

Protocol:add_handler{type = "FEAT", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		-- Players are able to create custom feats on client side. When
		-- the feat is performed, the client sends us all the information
		-- on the feat. We then reconstruct and perform the feat.
		local ok,anim,e1,v1,e2,v2,e3,v3,on = args.packet:read("string",
			"string", "float", "string", "float", "string", "float", "bool")
		if ok then
			if anim == "" or not Featanimspec:find{name = anim} then anim = nil end
			if e1 == "" or not Feateffectspec:find{name = e1} then e1 = nil end
			if e2 == "" or not Feateffectspec:find{name = e2} then e2 = nil end
			if e3 == "" or not Feateffectspec:find{name = e3} then e3 = nil end
			local feat = Feat{animation = anim, effects = {
				e1 and {e1, math.max(1, math.min(100, v1))},
				e2 and {e2, math.max(1, math.min(100, v2))},
				e3 and {e3, math.max(1, math.min(100, v3))}}}
			feat:perform{stop = not on, user = player}
		end
	end
end}

Protocol:add_handler{type = "INVENTORY_CLOSED", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,id = args.packet:read("uint32")
		if not ok then return end
		local inv = Inventory:find{id = id}
		if not inv or inv == player.inventory then return end
		inv:unsubscribe{object = player}
	end
end}

Protocol:add_handler{type = "JUMP", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		player:jump()
	end
end}

Protocol:add_handler{type = "MOVE_ITEM", func = function(args)
	-- Find the player.
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the source and destination types.
	local ok,src,dst = args.packet:read("uint8", "uint8")
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
		return Actions:move_from_world_to_inv(player, srcid, dstid, dstslot)
	elseif dst == moveitem.WORLD then
		return Actions:move_from_inv_to_world(player, srcid, srcslot)
	else
		return Actions:move_from_inv_to_inv(player, srcid, srcslot, dstid, dstslot)
	end
end}

Protocol:add_handler{type = "PLAYER_BLOCK", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	local ok,v = args.packet:read("bool")
	if ok then player:set_block(v) end
end}

Protocol:add_handler{type = "PLAYER_MOVE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,v = args.packet:read("int8")
		if ok then
			if v > 0 then
				player:set_movement(1)
			elseif v < 0 then
				player:set_movement(-1)
			else
				player:set_movement(0)
			end
		end
	end
end}

Protocol:add_handler{type = "PLAYER_TURN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,x,y,z,w = args.packet:read("float", "float", "float", "float")
		if ok then
			local e = Quaternion(x, y, z, w).euler
			e[3] = math.min(player.spec.tilt_limit, e[3])
			e[3] = math.max(-player.spec.tilt_limit, e[3])
			player.tilt = Quaternion:new_euler{0, 0, e[3]}
			player.rotation = Quaternion:new_euler{e[1], e[2], 0}
		end
	end
end}

Protocol:add_handler{type = "RUN", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,on = args.packet:read("bool")
		if ok then
			player.running = on
			player:calculate_speed()
		end
	end
end}

Protocol:add_handler{type = "SKILLS", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		-- Read packet data.
		local ok,s,v = args.packet:read("string", "float")
		if not ok then return end
		player:set_skill(s, v)
	end
end}

Protocol:add_handler{type = "SHOOT", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local anim = nil
		local weapon = player:get_item{slot = "hand.R"}
		if not weapon or weapon.spec.categories["melee"] then
			anim = "right hand"
		elseif weapon.spec.categories["ranged"] then
			anim = "ranged"
		elseif weapon.spec.categories["throwable"] then
			anim = "throw"
		elseif weapon.spec.categories["build"] then
			anim = "build"
		end
		if anim then
			local feat = Feat{animation = anim}
			feat:perform{stop = args.stop, user = player}
		end
	else
		player:respawn()
	end
end}

Protocol:add_handler{type = "STRAFE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,val = args.packet:read("int8")
		if ok then
			player:set_strafing(val / 127)
		end
	end
end}

Protocol:add_handler{type = "THROW", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if not player.dead then
		local ok,inv,slot = args.packet:read("uint32", "uint32")
		if ok then player:throw(inv, slot) end
	end
end}

Protocol:add_handler{type = "USE", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	if player.dead then return end
	-- Read the source type.
	local ok,inv,type = args.packet:read("uint32", "uint8")
	if not ok then return end
	-- Read the source slot.
	local slot
	if type == 0 then
		ok,slot = args.packet:resume("uint32")
	else
		ok,slot = args.packet:resume("string")
	end
	if not ok then return end
	-- Use the object.
	local object = player:find_target(inv, slot)
	if object and object.use_cb then
		object:use_cb(player)
	end
end}
