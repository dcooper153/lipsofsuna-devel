Message{
	name = "create character",
	client_to_server_encode = function(self, char)
		return {
			"string", char.name,
			"string", char.race,
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
		ok1,style.name,style.race = packet:read("string", "string")
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
		local account = Server.accounts_by_client[client]
		if not account then return end
		-- Make sure not created already.
		local player = Server:get_player_by_client(client)
		if player then return end
		-- Get the actor spec of the character.
		local spec = Actorspec:find{name = char.race .. "-player"}
		if not spec then return end
		-- Create the character.
		local player = Player{
			account = account,
			body_scale = char.body_scale,
			body_style = char.body_style,
			eye_color = char.eye_color,
			eye_style = char.eye_style,
			face_style = char.face_style,
			hair_color = char.hair_color,
			hair_style = char.hair_style,
			head_style = char.head_style,
			name = (char.name ~= "" and char.name or "Player"),
			random = true,
			skin_color = char.skin_color,
			skin_style = char.skin_style,
			spec = spec}
		Game.messaging:server_event("accept character", client)
		Server:spawn_player(player, client, char.spawn_point)
		Server.serialize:save_object(player)
	end}
