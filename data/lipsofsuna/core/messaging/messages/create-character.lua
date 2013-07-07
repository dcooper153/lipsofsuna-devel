-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Player = require("core/objects/player")

Main.messaging:register_message{
	name = "create character",
	client_to_server_encode = function(self, char)
		return {
			"string", char.name,
			"string", char.race,
			-- Animation profile.
			"string", char.animation_profile,
			-- Body style.
			"uint8", char.body_scale,
			"uint8", char.body_style[1],
			"uint8", char.body_style[2],
			"uint8", char.body_style[3],
			"uint8", char.body_style[4],
			"uint8", char.body_style[5],
			"uint8", char.body_style[6],
			"uint8", char.body_style[7],
			"uint8", char.body_style[8],
			"uint8", char.body_style[9],
			"uint8", char.body_style[10],
			-- Head style.
			"string", char.head_style,
			-- Eye style.
			"string", char.eye_style,
			"uint8", char.eye_color[1],
			"uint8", char.eye_color[2],
			"uint8", char.eye_color[3],
			-- Face style.
			"uint8", char.face_style[1],
			"uint8", char.face_style[2],
			"uint8", char.face_style[3],
			"uint8", char.face_style[4],
			"uint8", char.face_style[5],
			"uint8", char.face_style[6],
			"uint8", char.face_style[7],
			"uint8", char.face_style[8],
			"uint8", char.face_style[9],
			"uint8", char.face_style[10],
			"uint8", char.face_style[11],
			"uint8", char.face_style[12],
			"uint8", char.face_style[13],
			"uint8", char.face_style[14],
			"uint8", char.face_style[15],
			-- Hair style.
			"string", char.hair_style,
			"uint8", char.hair_color[1],
			"uint8", char.hair_color[2],
			"uint8", char.hair_color[3],
			-- Skin style.
			"string", char.skin_style,
			"uint8", char.skin_color[1],
			"uint8", char.skin_color[2],
			"uint8", char.skin_color[3],
			-- Spawn point
			"string", char.spawn_point}
	end,
	client_to_server_decode = function(self, packet)
		-- Reconstruct the character style.
		local style,ok1,ok2 = {}
		ok1,style.name,style.race,style.animation_profile = packet:read("string", "string", "string")
		if not ok1 then return end
		ok1,style.body_scale = packet:resume("uint8")
		ok2,style.body_style = packet:resume_table(
			"uint8", "uint8", "uint8", "uint8", "uint8",
			"uint8", "uint8", "uint8", "uint8", "uint8")
		if not ok1 or not ok2 then return end
		ok1,style.head_style = packet:resume("string")
		if not ok1 then return end
		ok1,style.eye_style = packet:resume("string")
		ok2,style.eye_color = packet:resume_table("uint8", "uint8", "uint8")
		if not ok1 or not ok2 then return end
		ok1,style.face_style = packet:resume_table(
			"uint8", "uint8", "uint8", "uint8", "uint8",
			"uint8", "uint8", "uint8", "uint8", "uint8",
			"uint8", "uint8", "uint8", "uint8", "uint8")
		if not ok1 then return end
		ok1,style.hair_style = packet:resume("string")
		ok2,style.hair_color = packet:resume_table("uint8", "uint8", "uint8")
		if not ok1 or not ok2 then return end
		ok1,style.skin_style = packet:resume("string")
		ok2,style.skin_color = packet:resume_table("uint8", "uint8", "uint8")
		if not ok1 or not ok2 then return end
		ok1,style.spawn_point = packet:resume("string")
		if not ok1 then return end
		return {style}
	end,
	client_to_server_handle = function(self, client, char)
		-- Make sure the client has been authenticated.
		local account = Server.account_database:get_account_by_client(client)
		if not account then return end
		-- Make sure not created already.
		local player = Server:get_player_by_client(client)
		if player then return end
		-- Get the actor spec of the character.
		local spec = Actorspec:find{name = char.race .. "-player"}
		if not spec then return end
		-- Create the character.
		local player = Player(Main.objects)
		player:set_spec(spec)
		player:randomize()
		player.account = account
		player.animation_profile = char.animation_profile
		player.body_scale = char.body_scale
		player.body_style = char.body_style
		player.eye_color = char.eye_color
		player.eye_style = char.eye_style
		player.face_style = char.face_style
		player.hair_color = char.hair_color
		player.hair_style = char.hair_style
		player.head_style = char.head_style
		player.name = (char.name ~= "" and char.name or "Player")
		player.skin_color = char.skin_color
		player.skin_style = char.skin_style
		-- Spawn the character.
		Server:spawn_player(player, client, char.spawn_point)
		Server.object_database:save_object(player)
	end}
