-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "sidestep",
	client_to_server_predict = function(self, value)
		local o = Client.player_object
		if not o then return end
		if not o.prediction then return end
		local vel = o:get_rotation() * Vector(1,0,0) * (value * o.spec.speed_walk)
		o.prediction:set_target_velocity(vel)
	end,
	client_to_server_encode = function(self, value)
		return {"int8", value * 127}
	end,
	client_to_server_decode = function(self, packet)
		local ok,value = packet:read("int8")
		if not ok then return end
		return {value / 127}
	end,
	client_to_server_handle = function(self, client, value)
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		player:set_strafing(value)
	end}
