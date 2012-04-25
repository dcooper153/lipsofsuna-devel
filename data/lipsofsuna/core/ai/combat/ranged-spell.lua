Aiactionspec{
	name = "ranged spell",
	categories = {"combat", "offensive"},
	calculate = function(self, args)
		if not args.spec.can_cast_ranged then return end
		if args.aim < 0.8 then return end
		args.feat_spell_ranged = self:find_best_feat{category = "ranged spell", target = self.target}
		if args.feat_spell_ranged then return (args.dist > args.hint) and 4 or 2 end
	end,
	perform = function(self, args)
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_ranged_tilt()
		args.feat_spell_ranged:perform{user = self.object}
		self.action_timer = 1
	end}
