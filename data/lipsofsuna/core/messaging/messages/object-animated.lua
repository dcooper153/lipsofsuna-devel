Message{
	name = "object animated",
	server_to_client_encode = function(self, id, name, time, variant)
		return {"uint32", id, "string", name, "float", time, "uint8", variant or 0}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,name,time,variant = packet:read("uint32", "string", "float", "uint8")
		if not ok then return end
		return {id, name, time, variant}
	end,
	server_to_client_handle = function(self, id, name, time, variant)
		-- Get the object.
		local o = Main.objects:find_by_id(id)
		if not o then return end
		-- Set the client side animation.
		-- FIXME: Weapon slot assumption to hack functional charge attacks.
		local weapon = o and o.inventory:get_object_by_slot("hand.R")
		o.render:add_animation(name, time, variant, weapon)
	end}
