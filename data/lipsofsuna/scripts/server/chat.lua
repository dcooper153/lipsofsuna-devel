ChatCommand = Class()
ChatCommand.dict_id = {}

--- Registers a new chat command.
-- @param clss Chat command class.
-- @param args Arguments.<ul>
--   <li>func: Handler function.</li>
--   <li>pattern: Pattern to match.</li></ul>
-- @return New chat command.
ChatCommand.new = function(clss, args)
	local self = Class.new(clss, args)
	self.id = #clss.dict_id + 1
	clss.dict_id[self.id] = self
	return self
end

-- Spawn item.
ChatCommand{pattern = "^/spawn item (.*)$", func = function(player, matches)
	local spec = Itemspec:find{name = matches[1]}
	if not spec then return end
	Item{
		spec = spec,
		position = player.position,
		random = true,
		realized = true}
end}

-- Spawn obstacles.
ChatCommand{pattern = "^/spawn obstacle (.*)$", func = function(player, matches)
	local spec = Obstaclespec:find{name = matches[1]}
	if not spec then return end
	Obstacle{
		spec = spec,
		position = player.position,
		realized = true}
end}

-- Spawn species.
ChatCommand{pattern = "^/spawn species (.*)$", func = function(player, matches)
	local spec = Species:find{name = matches[1]}
	if not spec then return end
	Creature{
		spec = spec,
		position = player.position,
		random = true,
		realized = true}
end}

-- Suicide.
ChatCommand{pattern = "^/suicide$", func = function(player, matches)
	player:die()
end}

-- Teleportation.
ChatCommand{pattern = "^/teleport (.*)$", func = function(player, matches)
	if player:teleport{marker = matches[1]} then
		player:send{packet = Packet(packets.MESSAGE, "string", "/teleport: Teleported to " .. matches[1] .. ".")}
	else
		player:send{packet = Packet(packets.MESSAGE, "string", "/teleport: Map marker " .. matches[1] .. " doesn't exist.")}
	end
end}

-- Any other command.
ChatCommand{pattern = "^(/[^ ]*).*", func = function(player, matches)
	player:send{packet = Packet(packets.MESSAGE, "string", "Unrecognized command.")}
end}

-- Normal chat.
ChatCommand{pattern = ".*", func = function(player, matches)
	player:say(matches[1])
end}

Protocol:add_handler{type = "CHAT", func = function(args)
	local player = Player:find{client = args.client}
	if not player then return end
	local ok,msg = args.packet:read("string")
	if ok then
		for k,v in ipairs(ChatCommand.dict_id) do
			local matches = string.match(msg, v.pattern)
			if matches then return v.func(player, {matches}) end
		end
	end
end}
