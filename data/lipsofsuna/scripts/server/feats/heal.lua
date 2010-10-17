local feat = Feat:find{name = "heal"}
feat.func = function(self, args)
	Combat:apply_spell_hit{
		attacker = args.user,
		feat = self,
		point = args.user.position,
		target = args.user}
end
