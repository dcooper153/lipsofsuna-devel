Aiactionspec{
	name = "ranged",
	categories = {["combat"] = true, ["offensive"] = true},
	calculate = function(self, args)
		-- Make sure that the actor can use ranged.
		if not args.attack then return end
		if not args.spec.can_ranged then return end
		-- Check for good aim.
		if args.aim < 0.8 then return end
		-- Check for a ranged weapon.
		--
		-- If the actor has a weapon wielded, it must be a ranged weapon
		-- for ranged attacks to be possible. If no weapon is wielded,
		-- ranged attacks may be possible for certain actors, such as
		-- turrets that have natural firing ability.
		if args.weapon and not args.weapon.spec.categories["ranged"] then return end
		-- Check for an applicable feat.
		--
		-- The actor must know a ranged feat suitable for the weapon or
		-- a natural ranged feat, such as turret fire, if no weapon is wielded.
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
