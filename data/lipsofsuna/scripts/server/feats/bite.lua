local feat = Feat:find{name = "bite"}
feat.func = function(self, args)
	Attack:sweep{user = args.user, slot = "mouth", func = function(f, r)
		Combat:apply_melee_hit{
			attacker = args.user,
			feat = self,
			point = r.point,
			target = r.object}
	end}
end
