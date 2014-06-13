local AiActionSpec = require("core/specs/aiaction")

AiActionSpec{
	name = "move backward",
	categories = {["combat"] = true, ["defensive"] = true},
	calculate = function(self, args)
		if not args.allow_backward and not args.allow_backward_jump then return end
		if args.dist > 2 * args.hint then return end
		return (args.dist < 0.3 * args.hint) and 4 or 1
	end,
	perform = function(self, args)
		self.object:set_block(false)
		self.object:set_movement(-0.5)
		self.object:set_strafing(0)
		self.action_timer = math.random(2, 4)
	end}
