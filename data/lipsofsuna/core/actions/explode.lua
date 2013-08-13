-- Self-destruction.
-- The actor explodes after the animation has played.
Actionspec{
	name = "explode",
	categories = { ["melee"] = true },
	start = function(action)
		if action.object.cooldown then return end
		Main.vision:object_effect(action.object, "selfdestruct1")
		action.delay = action.object.spec.timing_attack_explode * 0.02
		action.timer = 0
		return true
	end,
	update = function(action, secs)
		action.object.cooldown = 1
		action.timer = action.timer + secs
		if action.timer < action.delay then return true end
		action.object:die()
		Utils:explosion(action.object:get_position())
	end,
	get_range = function(action)
		return 3
	end,
	get_score = function(action)
		return 1
	end}
