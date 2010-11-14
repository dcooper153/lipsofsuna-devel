local feat = Feat:find{name = "attack-throw"}
feat.func = function(self, args)
	-- Get weapon.
	local slots = Slots:find{owner = args.user}
	local weapon = slots:get_object{slot = "hand.R"}
	-- Perform ranged attack.
	Attack:ranged{user = args.user, delay = 2, start = 0.5, func = function(user, point)
		weapon:fire{
			collision = false,
			feat = self,
			point = point + Vector(0.2, 0.3, -1.5),
			owner = args.user,
			speed = 8,
			timer = 3}
	end}
end
