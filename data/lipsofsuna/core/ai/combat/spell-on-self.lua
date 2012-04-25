Aiactionspec{
	name = "spell on self",
	categories = {"combat", "defensive"},
	calculate = function(self, args)
		if not args.spec.can_cast_self then return end
		if args.dist < 2 * args.hint then return end
		args.feat_spell_self = self:find_best_feat{category = "spell on self", target = self.object}
		if args.feat_spell_self then return 4 end
	end,
	perform = function(self, args)
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = Quaternion()
		args.feat_spell_self:perform{user = self.object}
		self.action_timer = 1
	end}
