Aiactionspec{
	name = "melee",
	categories = {["combat"] = true, ["offensive"] = true},
	calculate = function(self, args)
		-- Make sure that the actor can use melee.
		if not args.attack then return end
		if not args.spec.can_melee then return end
		-- Check for good aim.
		if args.aim < 0.8 then return end
		-- Check for a melee weapon or bare-handed.
		if args.weapon and not args.weapon.spec.categories["melee"] then return end
		-- Check for an applicable feat.
		args.action_melee = self:find_best_action{category = "melee", target = self.target, weapon = args.weapon}
		if args.action_melee then return 4 end
	end,
	perform = function(self, args)
		if args.diff.y > 1 and args.spec.allow_jump then self.object:action("jump") end
		self.object:set_block(false)
		if args.spec.ai_enable_backstep and args.dist < 0.3 * args.hint then
			self.object:set_movement(-1)
		elseif args.spec.ai_enable_walk and args.dist > 0.6 * args.hint then
			self.object:set_movement(1)
		else
			self.object:set_movement(0)
		end
		self.object.tilt = self:calculate_melee_tilt()
		self.object:set_strafing(0)
		self.object:action(args.action_melee.name)
		self.action_timer = 0.5
	end}
