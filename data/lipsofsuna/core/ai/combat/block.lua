local AiActionSpec = require("core/specs/aiaction")

AiActionSpec{
	name = "block",
	categories = {["combat"] = true, ["defensive"] = true},
	calculate = function(self, args)
		if not args.spec.ai_enable_block then return end
		if args.dist > args.hint then return end
		return 1
	end,
	perform = function(self, args)
		self.object:set_block(true)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_melee_tilt()
		self.action_timer = math.random(4, 8)
	end}
