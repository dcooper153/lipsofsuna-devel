Message{
	name = "object attack",
	server_to_client_encode = function(self, id, move, variant)
		return {"uint32", id, "string", move, "uint8", variant}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,slot,move,variant = packet:read("uint32", "string", "uint8")
		if not ok then return end
		return {id, move, variant}
	end,
	server_to_client_handle = function(self, id, move, variant)
		-- Find the object.
		local obj = Object:find{id = id}
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
