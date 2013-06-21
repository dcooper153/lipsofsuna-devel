-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object attack",
	server_to_client_encode = function(self, id, move, variant)
		return {"uint32", id, "string", move, "uint8", variant}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,move,variant = packet:read("uint32", "string", "uint8")
		if not ok then return end
		return {id, move, variant}
	end,
	server_to_client_handle = function(self, id, move, variant)
		-- Find the object.
		local obj = Main.objects:find_by_id(id)
		if not obj then return end
		if not obj.spec then return end
		-- Add a speedline effect for the weapon.
		local weapon = obj:get_weapon()
		if weapon and weapon.spec.effect_attack_speedline then
			local anchor = obj.render:get_equipment_anchor(weapon)
			if anchor then
				anchor:add_speedline{delay = 0.1, duration = 0.3}
			end
		end
		-- Play an animation based on the movement direction.
		obj.render:add_animation("attack " .. move, 0.0, variant, weapon)
	end}
