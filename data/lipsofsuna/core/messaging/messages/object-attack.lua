-- FIXME: This is never called anywhere!
Message{
	name = "object attack",
	server_to_client_encode = function(self, id, name, move)
		return {"uint32", id, "string", name, "uint8", move}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,name,move = packet:read("uint32", "string", "uint8")
		if not ok then return end
		return {id, name, move}
	end,
	server_to_client_handle = function(self, id, name, move)
		-- Redundant in single player.
		if Server.initialized then return end
		-- Get the object.
		local obj = Object:find{id = id}
		if not obj then return end
		if not obj.spec then return end
		if not obj.spec.animations then return end
		-- Find the feat type.
		local anim = Feattypespec:find{name = name}
		if not anim then return end
		-- Add a speedline effect for the weapon.
		local weapon = anim.slot and obj.inventory:get_object_by_slot(anim.slot)
		if weapon and weapon.spec.effect_attack_speedline then
			weapon:add_speedline{delay = 0.1, duration = 0.3}
		end
		-- Determine the character animation.
		local map = {"attack stand", "attack left", "attack right", "attack back", "attack front"}
		local animation = map[move] or "attack stand"
		-- Play the animation.
		obj.render:add_animation(animation, 0, 0, weapon)
	end}
