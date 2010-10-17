local feat = Feat:find{name = "spinattack"}
feat.func = function(self, args)
	local slots = Slots:find{owner = args.user}
	local weapon = slots and slots:get_object{slot = "hand.R"}
	Attack:sweep{user = args.user, slot = "handr", func = function(f, r)
		Combat:apply_melee_hit{
			attacker = args.user,
			feat = self,
			point = r.point,
			target = r.object,
			weapon = weapon}
	end}
end
