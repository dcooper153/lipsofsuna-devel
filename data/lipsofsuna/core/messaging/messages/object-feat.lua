-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object feat",
	server_to_client_encode = function(self, id, name, move)
		return {"uint32", id, "string", name, "uint8", move}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,name,move = packet:read("uint32", "string", "uint8")
		if not ok then return end
		return {id, name, move}
	end,
	server_to_client_handle = function(self, id, name, move)
		-- Find the object.
		local obj = Main.objects:find_by_id(id)
		if not obj then return end
		if not obj.spec then return end
		if not obj.spec.animations then return end
		-- Find the feat type.
		local anim = Feattypespec:find{name = name}
		if not anim then return end
		-- Choose the animation based on the movement direction.
		local animation = anim.animation or "attack"
		if anim.directional then
			local map = {" stand", " left", " right", " back", " front"}
			animation = animation .. (map[move] or " stand")
		end
		-- Add a speedline effect for the weapon.
		local weapon = anim.slot and obj.inventory:get_object_by_slot(anim.slot)
		if weapon and weapon.spec.effect_attack_speedline then
			local anchor = obj.render:get_equipment_anchor(weapon)
			if anchor then
				anchor:add_speedline{delay = 0.1, duration = 0.3}
			end
		end
		-- Play the animation.
		-- TODO: Animation variant number.
		obj.render:add_animation(animation, 0.0, nil, weapon)
	end}
