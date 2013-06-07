local Feat = require("arena/feat")

Message{
	name = "cast spell",
	client_to_server_encode = function(self, anim, e1, v1, e2, v2, e3, v3, on)
		return {"string", anim,
			"string", e1, "float", v1,
			"string", e2, "float", v2,
			"string", e2, "float", v3, "bool", on}
	end,
	client_to_server_decode = function(self, packet)
		local ok,anim,e1,v1,e2,v2,e3,v3,on = packet:read("string",
			"string", "float", "string", "float", "string", "float", "bool")
		if not ok then return end
		return {anim, e1, v1, e2, v2, e3, v3, on}
	end,
	client_to_server_handle = function(self, client, anim, e1, v1, e2, v2, e3, v3, on)
		-- Get the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Players are able to create custom feats on client side. When
		-- the feat is performed, the client sends us all the information
		-- on the feat. We then reconstruct and perform the feat.
		if anim == "" or not Feattypespec:find{name = anim} then anim = nil end
		if e1 == "" or not Feateffectspec:find{name = e1} then e1 = nil end
		if e2 == "" or not Feateffectspec:find{name = e2} then e2 = nil end
		if e3 == "" or not Feateffectspec:find{name = e3} then e3 = nil end
		local feat = Feat(anim, {e1 and {e1, 1}, e2 and {e2, 1}, e3 and {e3, 1}})
		-- Inform the client about failures.
		local ok,msg = feat:usable{user = player}
		if on and not ok then
			player:send_message(msg)
		end
		-- Perform the feat.
		feat:perform{stop = not on, user = player}
	end}
