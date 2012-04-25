Aiactionspec{
	name = "ranged",
	categories = {"combat", "offensive"},
	calculate = function(self, args)
		-- Make sure that the actor can use ranged.
		if not args.attack then return end
		if not args.spec.can_ranged then return end
		-- Check for good aim.
		if args.aim < 0.8 then return end
		-- Check for a ranged weapon or bare-handed.
		if not args.weapon then return end
		if not args.weapon.spec.categories["ranged"] then return end
		-- Check for an applicable feat.
		args.feat_ranged = self:find_best_feat{category = "ranged", target = self.target, weapon = args.weapon}
		if args.feat_ranged then return 4 end
	end,
	perform = function(self, args)
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_ranged_tilt()
		args.feat_ranged:perform{user = self.object}
		self.action_timer = 1
	end}
