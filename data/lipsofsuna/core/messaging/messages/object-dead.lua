Message{
	name = "object dead",
	server_to_client_encode = function(self, id, value)
		return {"uint32", id, "bool", value}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,value = packet:read("uint32", "bool")
		if not ok then return end
		return {id, value}
	end,
	server_to_client_handle = function(self, id, value)
		-- Get the object.
		local obj = Object:find{id = id}
		if not obj then return end
		-- Update death status.
		if obj.dead == value then return end
		obj.dead = value
		if obj == Client.player_object then
			Client:set_player_dead(value)
		end
	end}
