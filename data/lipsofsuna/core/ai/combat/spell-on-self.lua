local AiActionSpec = require("core/specs/aiaction")

AiActionSpec{
	name = "self spell",
	categories = {["combat"] = true, ["defensive"] = true},
	calculate = function(self, args)
		if not args.spec.can_cast_self then return end
		if args.dist < 2 * args.hint then return end
		args.action_spell_self = self:find_best_action{category = "self spell", target = self.object}
		if args.action_spell_self then return 4 end
	end,
	perform = function(self, args)
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = Quaternion()
		self.object:action(args.action_spell_self.name)
		self.action_timer = 1
	end}
