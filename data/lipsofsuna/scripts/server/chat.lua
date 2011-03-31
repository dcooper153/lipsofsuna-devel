ChatCommand = Class()
ChatCommand.dict_id = {}

--- Registers a new chat command.
-- @param clss Chat command class.
-- @param args Arguments.<ul>
--   <li>func: Handler function.</li>
--   <li>pattern: Pattern to match.</li>
--   <li>permission: Permission level.</ul>
-- @return New chat command.
ChatCommand.new = function(clss, args)
	local self = Class.new(clss, args)
	self.id = #clss.dict_id + 1
	clss.dict_id[self.id] = self
	return self
end

-- Grant admin privileges.
ChatCommand{pattern = "^/grant admin ([a-zA-Z0-9]*)$", permission = "admin", func = function(player, matches)
	if not Config.inst.admins[matches] then
		Config.inst.admins[matches] = true
		Config.inst:save()
		player:send("Admin privileges have been granted to " .. matches)
		local affected = Account.dict_name[matches]
		if affected and affected.client then
			Network:send{client = affected.client, packet = Packet(packets.ADMIN_PRIVILEGE, "bool", true)}
		end
	else
		player:send("Admin privileges have already been granted to " .. matches)
	end
end}

-- Revoke admin privileges.
ChatCommand{pattern = "^/revoke admin ([a-zA-Z0-9]*)$", permission = "admin", func = function(player, matches)
	if Config.inst.admins[matches] then
		Config.inst.admins[matches] = nil
		Config.inst:save()
		player:send("Admin privileges have been revoked from " .. matches)
		local affected = Account.dict_name[matches]
		if affected and affected.client then
			Network:send{client = affected.client, packet = Packet(packets.ADMIN_PRIVILEGE, "bool", false)}
		end
	else
		player:send("Admin privileges have already been revoked from " .. matches)
	end
end}

-- Spawn item.
ChatCommand{pattern = "^/spawn item (.*)$", permission = "admin", func = function(player, matches)
	local spec = Itemspec:find{name = matches[1]}
	if not spec then return end
	Item{
		spec = spec,
		position = player.position,
		random = true,
		realized = true}
end}

-- Spawn obstacles.
ChatCommand{pattern = "^/spawn obstacle (.*)$", permission = "admin", func = function(player, matches)
	local spec = Obstaclespec:find{name = matches[1]}
	if not spec then return end
	Obstacle{
		spec = spec,
		position = player.position,
		realized = true}
end}

-- Spawn species.
ChatCommand{pattern = "^/spawn species (.*)$", permission = "admin", func = function(player, matches)
	local spec = Species:find{name = matches[1]}
	if not spec then return end
	Creature{
		spec = spec,
		position = player.position,
		random = true,
		realized = true}
end}

-- Suicide.
ChatCommand{pattern = "^/suicide$", permission = "player", func = function(player, matches)
	player:die()
end}

-- Teleportation.
ChatCommand{pattern = "^/teleport (.*)$", permission = "admin", func = function(player, matches)
	if player:teleport{marker = matches[1]} then
		player:send{packet = Packet(packets.MESSAGE, "string", "/teleport: Teleported to " .. matches[1] .. ".")}
	else
		player:send{packet = Packet(packets.MESSAGE, "string", "/teleport: Map marker " .. matches[1] .. " doesn't exist.")}
	end
end}

-- Any other command.
ChatCommand{pattern = "^(/[^ ]*).*", permission = "player", func = function(player, matches)
	player:send("Unrecognized command.")
end}

-- Normal chat.
ChatCommand{pattern = ".*", permission = "player", func = function(player, matches)
	player:say(matches[1])
end}

Protocol:add_handler{type = "CHAT", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	local ok,msg = args.packet:read("string")
	if ok then
		for k,v in ipairs(ChatCommand.dict_id) do
			local matches = string.match(msg, v.pattern)
			if matches then
				if v.permission == "admin" and not player.admin then
					return player:send("You have no permission to do that.")
				end
				return v.func(player, {matches})
			end
		end
	end
end}
