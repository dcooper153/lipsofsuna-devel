local ActionSpec = require("core/specs/action")
local Damage = require("arena/damage")
local Physics = require("system/physics")

ActionSpec{
	name = "circle cut",
	categories = { ["melee"] = true },
	start = function(action)
		-- Initialize timing.
		action.time = 1
		action.step = -1
		action.damage = 0.8
		-- Jump.
		local o = action.object
		if not o.physics:get_ground() or o:get_burdened() then return end
		local v = o:get_velocity() * 0.2
		local f = o.spec.mass * o.spec.jump_force
		o.physics:jump(Vector(v.x, f, v.z))
		-- Play the start effect.
		Main.vision:object_effect(action.object, "swing1")
		action.object:animate("attack circle cut", true)
		action.object:action("jump")
		-- Enable effect-over-time updates.
		return true
	end,
	update = function(action, secs)
		-- Wait for the delay to end.
		action.object.cooldown = 1.4
		action.time = action.time - secs
		while action.time < 0 do
			action.time = action.time + 0.1
			action.step = action.step + 1
			-- Cast a downward attack ray.
			local i = action.step
			local a = -2.5 * math.pi * (i - 1) / 10
			local b = -2.5 * math.pi * i / 10
			local ctr = action.object.spec.aim_ray_center
			local ray1 = Vector(0, math.cos(a) - 0.5, math.sin(a)):multiply(action.object.spec.aim_ray_end / 5)
			local ray2 = Vector(0, math.cos(b) - 0.5, math.sin(b)):multiply(action.object.spec.aim_ray_end / 5)
			local src = action.object:transform_local_to_global(ctr + ray1)
			local dst = action.object:transform_local_to_global(ctr + ray2)
			local r = Physics:cast_sphere(src, dst, 0.5, nil, {action.object.physics})
			if not r then return i < 10 end
			-- Calculate the damage.
			local attacker = action.object
			local defender = r.object and Main.objects:find_by_id(r.object)
			local damage = Damage()
			damage:add_item_or_unarmed_modifiers(action.object:get_weapon(), action.object.skills)
			damage:add_knockback()
			damage:apply_attacker_physical_modifiers(attacker)
			damage:apply_attacker_charge(attacker:get_attack_charge())
			damage:apply_defender_armor(defender)
			damage:apply_defender_blocking(defender)
			damage:apply_defender_vulnerabilities(defender)
			damage:apply_attacker_charge(action.charge)
			damage:multiply(action.damage)
			action.damage = action.damage * 0.7
			-- Apply the damage.
			Main.combat_utils:apply_damage(attacker, defender, r.tile, damage, r.point)
		end
		return true
	end,
	get_score = function(action)
		return 1
	end}
