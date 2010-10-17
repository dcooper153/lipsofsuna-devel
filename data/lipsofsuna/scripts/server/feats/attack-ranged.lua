local feat = Feat:find{name = "attack-ranged"}
feat.func = function(self, args)
	-- Get weapon and ammo.
	local slots = Slots:find{owner = args.user}
	local weapon = slots:get_object{slot = "hand.R"}
	local ammo = args.user:split_items{name = "arrow"}
	-- Perform ranged attack.
	Attack:ranged{user = args.user, func = function(user, point)
		ammo:fire{
			collision = true,
			feat = self,
			point = point + Vector(0, 0.3, -1.5),
			owner = args.user,
			weapon = weapon}
	end}
end
