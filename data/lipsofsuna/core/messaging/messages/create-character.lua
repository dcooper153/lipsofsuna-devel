-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "create character",
	client_to_server_encode = function(self, char)
		local res = {}
		local add = function(t, v)
			table.insert(res, t)
			table.insert(res, v)
		end
		-- Name.
		add("string", char.name)
		add("string", char.race)
		-- Animation profile.
		add("string", char.animation_profile)
		-- Body style.
		add("uint8", char.body_scale)
		add("uint8", #char.body_sliders)
		for k,v in ipairs(char.body_sliders) do
			add("uint8", v)
		end
		-- Head style.
		add("string", char.head_style)
		-- Eye style.
		add("string", char.eye_style)
		add("uint8", char.eye_color[1])
		add("uint8", char.eye_color[2])
		add("uint8", char.eye_color[3])
		-- Face style.
		add("string", char.brow_style)
		add("string", char.face_style)
		add("string", char.mouth_style)
		add("uint8", #char.face_sliders)
		for k,v in ipairs(char.face_sliders) do
			add("uint8", v)
		end
		-- Hair style.
		add("string", char.hair_style)
		add("uint8", char.hair_color[1])
		add("uint8", char.hair_color[2])
		add("uint8", char.hair_color[3])
		-- Skin style.
		add("string", char.skin_style)
		add("uint8", char.skin_color[1])
		add("uint8", char.skin_color[2])
		add("uint8", char.skin_color[3])
		-- Spawn point.
		add("string", char.spawn_point)
		return res
	end,
	client_to_server_decode = function(self, packet)
		local char,count,ok1,ok2 = {}
		-- Name.
		ok1,char.name,char.race,char.animation_profile = packet:read("string", "string", "string")
		if not ok1 then return end
		-- Body style.
		ok1,char.body_scale,count = packet:resume("uint8", "uint8")
		ok2,char.body_sliders = packet:resume_table_count(count or 0, "uint8")
		if not ok1 or not ok2 then return end
		-- Head style.
		ok1,char.head_style = packet:resume("string")
		if not ok1 then return end
		-- Eye style.
		ok1,char.eye_style = packet:resume("string")
		ok2,char.eye_color = packet:resume_table("uint8", "uint8", "uint8")
		if not ok1 or not ok2 then return end
		-- Face style.
		ok1,char.brow_style,char.face_style,char.mouth_style = packet:resume("string", "string", "string")
		if not ok1 then return end
		ok1,count = packet:resume("uint8")
		ok2,char.face_sliders = packet:resume_table_count(count or 0, "uint8")
		if not ok1 or not ok2 then return end
		-- Hair style.
		ok1,char.hair_style = packet:resume("string")
		ok2,char.hair_color = packet:resume_table("uint8", "uint8", "uint8")
		if not ok1 or not ok2 then return end
		-- Skin style.
		ok1,char.skin_style = packet:resume("string")
		ok2,char.skin_color = packet:resume_table("uint8", "uint8", "uint8")
		if not ok1 or not ok2 then return end
		-- Spawn point.
		ok1,char.spawn_point = packet:resume("string")
		if not ok1 then return end
		return {char}
	end,
	client_to_server_handle = function(self, client, char)
		-- Make sure the client has been authenticated.
		local account = Main.accounts:get_account_by_client(client)
		if not account then return end
		-- Make sure not created already.
		local player = Server:get_player_by_client(client)
		if player then return end
		-- Create the character.
		local player = Main.objects:create_object_by_spec("Player", char.race .. "-player")
		if not player then return end
		player:randomize()
		player.account = account
		player.animation_profile = char.animation_profile
		player.body_scale = char.body_scale
		player.body_sliders = char.body_sliders
		player.brow_style = char.brow_style
		player.eye_color = char.eye_color
		player.eye_style = char.eye_style
		player.face_sliders = char.face_sliders
		player.face_style = char.face_style
		player.hair_color = char.hair_color
		player.hair_style = char.hair_style
		player.head_style = char.head_style
		player.mouth_style = char.mouth_style
		player.name = (char.name ~= "" and char.name or "Player")
		player.skin_color = char.skin_color
		player.skin_style = char.skin_style
		-- Spawn the character.
		Server:spawn_player(player, client, char.spawn_point)
		Server.object_database:save_object(player)
	end}
