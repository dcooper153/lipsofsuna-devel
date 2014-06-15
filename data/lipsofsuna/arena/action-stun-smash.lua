local ActionSpec = require("core/specs/action")
local Damage = require("core/combat/damage")
local Physics = require("system/physics")

ActionSpec{
	name = "stun smash",
	categories = { ["melee"] = true },
	start = function(action)
		-- Initialize timing.
		action.time = 0
		action.delay = 1.2
		-- Jump.
		local o = action.object
		if not o.physics:get_ground() or o:get_burdened() then return end
		local v = o:get_velocity() * 0.2
		local f = o.spec.mass * o.spec.jump_force * 0.9
		o.physics:jump(Vector(v.x, f, v.z))
		-- Play the start effect.
		Main.vision:object_effect(action.object, "swing1")
		action.object:animate("attack stun smash", true)
		action.object:action("jump")
		-- Enable effect-over-time updates.
		return true
	end,
	update = function(action, secs)
		-- Wait for the delay to end.
		action.object.cooldown = 1.4
		action.time = action.time + secs
		if action.time < action.delay then return true end
		-- Cast a downward attack ray.
		local ctr = action.object.spec.aim_ray_center
		local ray1 = Vector(0, 0.5, -action.object.spec.aim_ray_end / 4)
		local ray2 = Vector(0, -2, -action.object.spec.aim_ray_end / 3)
		local src = action.object:transform_local_to_global(ctr + ray1)
		local dst = action.object:transform_local_to_global(ctr + ray2)
		local r = Physics:cast_sphere(src, dst, 0.5, nil, {action.object.physics})
		if not r then
			local ray1 = Vector(0, -2, -action.object.spec.aim_ray_end / 3)
			local ray2 = Vector(0, -3.5, -action.object.spec.aim_ray_end / 2)
			local src = action.object:transform_local_to_global(ctr + ray1)
			local dst = action.object:transform_local_to_global(ctr + ray2)
			r = Physics:cast_sphere(src, dst, 0.5, nil, {action.object.physics})
			if not r then return end
		end
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
		-- Apply stunning.
		if defender and math.random() < 0.5 then
			defender:add_modifier("stun", 3 + 5 * math.random())
		end
		-- Apply the damage.
		Main.combat_utils:apply_damage(attacker, defender, r.tile, damage, r.point)
	end,
	get_score = function(action)
		return 1
	end}
