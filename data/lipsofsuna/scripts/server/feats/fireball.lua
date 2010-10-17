local feat = Feat:find{name = "fireball"}
feat.func = function(self, args)
	Thread(function()
		-- Start animation.
		Thread:sleep(0.2)
		-- Calculate trajectory.
		local p = args.user.position + args.user.rotation * Vector(0, 0, -1)
		local n = args.user:find_node{node = "#hand.R"}
		if n then
			p = p + args.user.rotation * n
		else
			p = p + Vector(0, 1, 0)
		end
		local v = args.user.rotation * Vector(0, 0, -10)
		-- Create object.
		local o = Object{model = "fireball1", rotation = args.user.rotation,
			position = p, velocity = v, physics = "rigid", realized = true}
		o.gravity = Vector(0, 0, 0)
		o:effect{effect = "fireball1"}
		-- Wait for collision.
		o.contact_cb = function(o, result)
			Particles:create(result.point, "explosion1")
			if result.object ~= nil then
				result.object:impulse{impulse = o.rotation * Vector(0, 0, -30)}
				Combat:apply_spell_hit{
					attacker = args.user,
					feat = self,
					point = result.point,
					target = result.object}
			end
			o.realized = false
		end
	end)
end
