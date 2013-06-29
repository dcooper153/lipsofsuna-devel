Aiactionspec{
	name = "touch spell",
	categories = {["combat"] = true, ["offensive"] = true},
	calculate = function(self, args)
		args.feat_spell_touch = nil
		if not args.spec.can_cast_touch then return end
		if args.dist > args.hint * 0.8 then return end
		if args.aim < 0.8 then return end
		args.action_spell_touch = self:find_best_action{category = "touch spell", target = self.target}
		if args.action_spell_touch then return 4 end
	end,
	perform = function(self, args)
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_melee_tilt()
		self.object:action(args.action_spell_touch.name)
		self.action_timer = 1
	end}
