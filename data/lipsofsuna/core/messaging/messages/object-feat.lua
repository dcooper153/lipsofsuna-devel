Message{
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
		local obj = Object:find{id = id}
		if not obj then return end
		if not obj.spec then return end
		if not obj.spec.animations then return end
		-- Find the feat type.
		local anim = Feattypespec:find{name = name}
		if not anim then return end
		-- Determine the character animation.
		-- The animation of the feat may be overridden by a weapon specific
		-- animation if there's a weapon in the slot used by the feat.
		local weapon = anim.slot and obj.inventory:get_object_by_slot(anim.slot)
		local animation = anim.animation
		if weapon and weapon.spec.animation_attack then
			animation = weapon.spec.animation_attack
		end
		-- Add a speedline effect for the weapon.
		if weapon and weapon.spec.effect_attack_speedline then
			local anchor = obj.render:get_equipment_anchor(weapon)
			if anchor then
				anchor:add_speedline{delay = 0.1, duration = 0.3}
			end
		end
		-- Melee feats may further override the animation since controls affect
		-- what move the player performs. This is indicated with the move variable.
		if move ~= 0 then
			local map = {
				"attack stand",
				"attack left",
				"attack right",
				"attack back",
				"attack front"}
			animation = map[move] or animation
		end
		-- Play the animation.
		obj.render:add_animation(animation)
	end}
