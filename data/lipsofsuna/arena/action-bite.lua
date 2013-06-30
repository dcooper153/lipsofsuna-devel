local Combat = require("core/server/combat")
local Physics = require("system/physics")

Actionspec{
	name = "bite",
	categories = { ["melee"] = true },
	start = function(action)
		-- Initialize timing.
		action.time = 0
		action.delay = action.object.spec.timing_attack_melee * 0.02
		-- Play the start effect.
		Server:object_effect(action.object, "swing1")
		action.object:animate("attack bite", true)
		-- Enable effect-over-time updates.
		return true
	end,
	update = function(action, secs)
		-- Wait for the delay to end.
		action.time = action.time + secs
		if action.time < action.delay then return true end
		-- Cast a straight attack ray.
		local src,dst = action.object:get_attack_ray()
		local r = Physics:cast_ray(src, dst, nil, {action.object.physics})
		-- Apply the damage.
		if r then
			local target = r.object and Main.objects:find_by_id(r.object)
			Combat:apply_melee_impact(action.object, nil, r.point, target, r.tile)
		end
	end,
	get_score = function(action)
		return 1
	end}
