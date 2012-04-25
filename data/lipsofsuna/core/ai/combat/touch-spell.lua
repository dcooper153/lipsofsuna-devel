Aiactionspec{
	name = "touch spell",
	categories = {"combat", "offensive"},
	calculate = function(self, args)
		args.feat_spell_touch = nil
		if not args.spec.can_cast_touch then return end
		if args.dist > args.hint then return end
		if args.aim < 0.8 then return end
		args.feat_spell_touch = self:find_best_feat{category = "spell on touch", target = self.target}
		if args.feat_spell_touch then return 4 end
	end,
	perform = function(self, args)
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_melee_tilt()
		args.feat_spell_touch:perform{user = self.object}
		self.action_timer = 1
	end}
