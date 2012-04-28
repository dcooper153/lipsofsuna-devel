Aiactionspec{
	name = "strafe",
	categories = {["combat"] = true, ["defensive"] = true},
	calculate = function(self, args)
		if args.allow_strafe_left then return 1 end
		if args.allow_strafe_left_jump then return 1 end
		if args.allow_strafe_right then return 1 end
		if args.allow_strafe_right_jump then return 1 end
	end,
	perform = function(self, args)
		local strafe_left = function()
			if args.allow_strafe_left or args.allow_strafe_left_jump then
				if args.allow_strafe_left_jump then self.object:jump() end
				self.object:set_block(false)
				self.object:set_strafing(-1)
				self.object:set_movement(0)
				self.action_timer = math.random(4, 8)
				return true
			end
		end
		local strafe_right = function()
			if allow_strafe_right or allow_strafe_right_jump then
				if allow_strafe_right_jump then self.object:jump() end
				self.object:set_block(false)
				self.object:set_strafing(1)
				self.object:set_movement(0)
				self.action_timer = math.random(4, 8)
				return true
			end
		end
		if math.random() > 0.5 then
			local r = strafe_left() or strafe_right()
		else
			local r = strafe_right() or strafe_left()
		end
	end}
