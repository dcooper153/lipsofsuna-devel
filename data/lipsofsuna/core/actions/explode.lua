-- Self-destruction.
-- The actor explodes after the animation has played.
Actionspec{
	name = "explode",
	start = function(feat, info, args)
		action.delay = action.object.spec.timing_attack_explode * 0.02
		action.timer = 0
		return true
	end,
	update = function(action, secs)
		action.timer = action.timer + secs
		if action.timer < action.delay then return true end
		action.feat:play_effects{user = action.object}
		action.object:die()
		Utils:explosion(action.object:get_position())
	end}
