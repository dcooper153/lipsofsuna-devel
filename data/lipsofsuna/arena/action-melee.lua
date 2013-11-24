local Damage = require("arena/damage")
local Physics = require("system/physics")

Actionspec{
	name = "melee",
	categories = { ["melee"] = true },
	start = function(action, move, charge)
		if action.object.cooldown then return end
		-- Choose the weapon sweep path.
		action.path = {Vector(0.1, 0, 0.75), Vector(-0.1, 0, 0.5), Vector(0.05, 0, 0.25), Vector(-0.05, 0, 0)}
		if not action.path then return end
		action.prev = Main.combat_utils:get_actor_attack_point(action.object, action.path[1])
		-- Play the start effect.
		Main.vision:object_effect(action.object, "swing1")
		Main.vision:object_event(action.object, "object attack", {move = "stand", variant = math.random(0, 255)})
		-- Enable effect-over-time updates.
		action.charge = charge or 0
		action.frame = 1
		action.time = 0
		action.duration = action.object.spec.timing_attack_melee * 0.02
		return true
	end,
	update = function(action, secs)
		local apply = function(r)
			local attacker = action.object
			local defender = r.object and Main.objects:find_by_id(r.object)
			local weapon = attacker:get_weapon()
			-- Calculate the damage.
			local damage = Damage()
			damage:add_item_or_unarmed_modifiers(weapon, attacker.skills)
			damage:add_knockback()
			damage:apply_attacker_physical_modifiers(attacker)
			damage:apply_attacker_charge(attacker:get_attack_charge())
			damage:apply_defender_armor(defender)
			damage:apply_defender_blocking(defender)
			damage:apply_defender_vulnerabilities(defender)
			damage:apply_attacker_charge(action.charge)
			-- Apply the damage.
			Main.combat_utils:apply_damage(attacker, defender, r.tile, damage, r.point)
		end
		-- Wait for the next sweep.
		action.object.cooldown = 1
		action.time = action.time + secs
		local frame = action.time / action.duration * #action.path + 1
		if frame <= action.frame then return true end
		action.frame = action.frame + 1
		-- Perform the sweep.
		local dst = Main.combat_utils:get_actor_attack_point(action.object, action.path[action.frame])
		local r = Physics:cast_ray(action.prev, dst, nil, {action.object.physics})
		action.prev = dst
		if r then return apply(r) end
		if action.path[action.frame] then return true end
		-- Cast a straight ray as a fallback.
		local src,dst = Main.combat_utils:get_attack_ray_for_actor(action.object)
		local r = Physics:cast_ray(src, dst, nil, {action.object.physics})
		if r then apply(r) end
	end,
	get_score = function(action)
		return 1
	end}
