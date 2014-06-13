local Damage = require("arena/damage")
local Physics = require("system/physics")

Actionspec{
	name = "bite",
	categories = { ["melee"] = true },
	start = function(action)
		if action.object.cooldown then return end
		-- Initialize timing.
		action.time = 0
		action.delay = action.object.spec.timing_attack_melee * 0.02
		-- Play the start animation.
		action.object:animate("attack bite", true)
		-- Play the start effect.
		local effect = action.object.spec:get_effect("attack bite")
		if effect then
			Main.vision:object_effect(action.object, effect.name)
		end
		-- Enable effect-over-time updates.
		return true
	end,
	update = function(action, secs)
		-- Wait for the delay to end.
		action.object.cooldown = 1
		action.time = action.time + secs
		if action.time < action.delay then return true end
		-- Cast a straight attack ray.
		local src,dst = Main.combat_utils:get_attack_ray_for_actor(action.object)
		local r = Physics:cast_ray(src, dst, nil, {action.object.physics})
		if not r then return end
		-- Calculate the damage.
		local attacker = action.object
		local defender = r.object and Main.objects:find_by_id(r.object)
		local damage = Damage()
		damage:add_unarmed_modifiers(attacker.skills)
		damage:add_knockback()
		damage:apply_attacker_physical_modifiers(attacker)
		damage:apply_attacker_charge(attacker:get_attack_charge())
		damage:apply_defender_armor(defender)
		damage:apply_defender_blocking(defender)
		damage:apply_defender_vulnerabilities(defender)
		damage:apply_attacker_charge(action.charge)
		-- Apply the damage.
		Main.combat_utils:apply_damage(attacker, defender, r.tile, damage, r.point)
	end,
	get_score = function(action)
		return 1
	end}
