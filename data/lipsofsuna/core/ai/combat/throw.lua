Aiactionspec{
	name = "throw",
	categories = {"combat", "offensive"},
	calculate = function(self, args)
		if not args.attack then return end
		if not args.spec.can_throw then return end
		if not args.weapon then return end
		if not args.weapon.spec.categories["throwable"] then return end
		if args.aim < 0.8 then return end
		args.feat_throw = self:find_best_feat{category = "throw", target = self.target, weapon = args.weapon}
		if args.feat_throw then return 4 end
	end,
	perform = function(self, args)
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_ranged_tilt()
		args.feat_throw:perform{user = self.object}
		self.action_timer = 1
	end}
