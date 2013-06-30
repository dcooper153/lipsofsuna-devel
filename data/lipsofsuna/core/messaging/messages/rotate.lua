-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "rotate",
	client_to_server_encode = function(self, turn, tilt)
		local o = Client.player_object
		if not o then return end
		o:set_rotation(Quaternion{euler = {turn, 0, 0}})
		o:set_tilt_angle(tilt)
		if not o.synced_turn_state or math.abs(o.synced_turn_state - turn) > math.pi/180 or
		   not o.synced_tilt_state or math.abs(o.synced_tilt_state - tilt) > math.pi/180 then
			o.synced_turn_state = turn
			o.synced_tilt_state = tilt
			return {"float", turn, "float", tilt}
		end
	end,
	client_to_server_decode = function(self, packet)
		local ok,turn,tilt = packet:read("float", "float")
		if not ok then return end
		return {turn, tilt}
	end,
	client_to_server_handle = function(self, client, turn, tilt)
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		tilt = math.min(player.spec.tilt_limit, tilt)
		tilt = math.max(-player.spec.tilt_limit, tilt)
		player:set_tilt_angle(tilt)
		player:set_rotation(Quaternion{euler = {turn, 0, 0}})
		Main.vision:object_event(player, "object-motion")
	end}
