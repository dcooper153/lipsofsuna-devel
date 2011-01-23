
Protocol:add_handler{type = "ADMIN_DELETE", func = function(args)
	args.packet:read()
	while true do
		local ok,id = args.packet:resume("uint32")
		if not ok then break end
		local obj = Object:find{id = id}
		if obj then
			obj:die()
			obj:purge()
		end
	end
end}

Protocol:add_handler{type = "ADMIN_DUPLICATE", func = function(args)
	args.packet:read()
	while true do
		local ok,id = args.packet:resume("uint32")
		if not ok then break end
		local obj = Object:find{id = id}
		if obj then
			Object:new{position = obj.position, rotation = obj.rotation, model = obj.model,
				static = true, realized = true}
		end
	end
end}

Protocol:add_handler{type = "ADMIN_SAVE", func = function(args)
	print("Saving world state...")
	Serialize:save()
	print("Done")
end}

Protocol:add_handler{type = "ADMIN_SHUTDOWN", func = function(args)
	Serialize:save()
	Program:shutdown()
end}

Protocol:add_handler{type = "ADMIN_SPAWN", func = function(args)
	local player = Player:find{client = args.client}
	local ok,typ,msg = args.packet:read("string", "string")
	if ok then
		if msg == "" then msg = nil end
		if typ == "item" then
			if not msg then return end
			local spec = Itemspec:find{name = msg}
			if not spec then return end
			Item{
				spec = spec,
				position = player.position,
				realized = true}
		elseif typ == "item-cat" then
			if not msg then return end
			local spec = Itemspec:random{category = msg}
			if not spec then return end
			Item{
				spec = spec,
				position = player.position,
				realized = true}
		elseif typ == "obstacle" then
			if not msg then return end
			local spec = Obstaclespec:find{name = msg}
			if not spec then return end
			Obstacle{
				spec = spec,
				position = player.position,
				realized = true}
		elseif typ == "obstacle-cat" then
			if not msg then return end
			local spec = Obstaclespec:random{category = msg}
			if not spec then return end
			Obstacle{
				spec = spec,
				position = player.position,
				realized = true}
		elseif typ == "creature" then
			if not msg then return end
			local spec = Species:find{name = msg}
			if not spec then return end
			Creature{
				spec = spec,
				position = player.position,
				realized = true}
		-- FIXME:
		elseif typ == "mover" then
			local object = Mover{
				model = msg or "porticullis-000",
				position = player.position,
				realized = true}
		elseif typ == "chest" then
			local object = Container{
				model = msg or "chest-000",
				position = player.position,
				realized = true}
			object:create_random_loot()
		elseif typ == "door" then
			local object = Door{
				mass = 10.0,
				model = msg or "door-004",
				position = player.position + Vector(0.0,0.5,0.0),
				rotation = player.rotation}
		elseif msg then
			local object = Object{
				model = msg,
				position = player.position,
				realized = true,
				static = true}
		end
	end
end}
