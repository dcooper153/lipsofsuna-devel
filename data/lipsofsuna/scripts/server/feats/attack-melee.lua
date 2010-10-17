local feat = Feat:find{name = "attack-melee"}
feat.func = function(self, args)
	local slots = Slots:find{owner = args.user}
	local weapon = slots and slots:get_object{slot = "hand.R"}
	Attack:sweep{user = args.user, slot = "hand.R", func = function(f, r)
		Combat:apply_melee_hit{
			attacker = args.user,
			feat = self,
			point = r.point,
			target = r.object,
			weapon = weapon}
	end}
end
