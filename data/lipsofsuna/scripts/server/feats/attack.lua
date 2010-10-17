local feat = Feat:find{name = "attack"}
feat.func = function(self, args)
	local slots = Slots:find{owner = args.user}
	local weapon = slots and slots:get_object{slot = "hand.R"}
	if not weapon or weapon.itemspec.categories["melee"] then
		Feat:perform{name = "attack-melee", stop = args.stop, user = args.user}
	elseif weapon.itemspec.categories["ranged"] then
		Feat:perform{name = "attack-ranged", stop = args.stop, user = args.user}
	elseif weapon.itemspec.categories["throwable"] then
		Feat:perform{name = "attack-throw", stop = args.stop, user = args.user}
	end
end
