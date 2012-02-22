require "common/chatcommand"

-- Dump debug information.
ChatCommand{pattern = "^/debug_dump$", func = function(matches)
	Program:debug_dump()
end}

-- Normal chat.
ChatCommand{pattern = ".*", func = function(matches)
	Network:send{packet = Packet(packets.PLAYER_CHAT, "string", matches[1])}
end}
