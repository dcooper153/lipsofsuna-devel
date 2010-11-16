local feat = Feat:find{name = "attack-melee"}
feat.func = function(self, args)
	local weapon = args.user:get_item{slot = "hand.R"}
	Attack:sweep{user = args.user, slot = "hand.R", func = function(f, r)
		Combat:apply_melee_hit{
			attacker = args.user,
			feat = self,
			point = r.point,
			target = r.object,
			tile = r.tile,
			weapon = weapon}
	end}
end
