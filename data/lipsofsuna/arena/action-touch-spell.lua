local Damage = require("arena/damage")
local Physics = require("system/physics")

Actionspec{
	name = "touch spell",
	categories = { ["touch spell"] = true },
	start = function(action, item)
		if action.object.cooldown then return end
		action.timer = 0
		action.delay = action.object.spec.timing_spell_touch * 0.02
		action.weapon = item or action.object:get_weapon()
		action.object:animate("spell touch", true)
		return true
	end,
	update = function(action, secs)
		-- Apply the casting delay.
		action.object.cooldown = 0.4
		action.timer = action.timer + secs
		if action.timer < action.delay then return true end
		-- Get the influences.
		local influences = Main.combat_utils:get_spell_influences_for_item(action.weapon)
		if not influences then return end
		-- TODO: Subtract stats.
		-- Cast a straight attack ray.
		local src,dst = action.object:get_attack_ray()
		local r = Physics:cast_ray(src, dst, nil, {action.object.physics})
		if not r then return end
		-- Apply the damage.
		local target = r.object and Main.objects:find_by_id(r.object)
		if target then
			local damage = Damage()
			damage:add_spell_influences(influences)
			damage:apply_defender_vulnerabilities(target)
			Main.combat_utils:apply_damage_to_actor(action.object, target, damage, r.point)
		else
			-- TODO: Damage terrain
		end
	end,
	get_score = function(action)
		return 1
	end}
