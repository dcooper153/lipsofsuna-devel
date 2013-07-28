Aiactionspec{
	name = "move forward",
	categories = {["combat"] = true, ["offensive"] = true},
	calculate = function(self, args)
		if not args.allow_forward and not args.allow_forward_jump then return end
		if args.dist < 0.5 * args.hint then return end
		return 1
	end,
	perform = function(self, args)
		if args.allow_forward_jump then self.object:action("jump") end
		self.object:set_block(false)
		self.object:set_movement(1)
		self.object:set_strafing(0)
		self.action_timer = math.random(1, 3)
	end}
