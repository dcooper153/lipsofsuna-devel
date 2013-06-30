Feateffectspec{
	name = "home",
	categories =
	{
		["follow"] = true,
		["spell"] = true
	},
	actions =
	{
		["self spell"] = true
	},
	description = "Creates a home location where you can respawn from",
	effect = "light1",
	icon = "sanctuary1",
	influences = {["home"] = 1},
	required_stats = {["willpower"] = 1},
	touch = function(self, args)
		if not args.object then return end
		if not args.object.account then return end
		local home = args.object:get_position()
		args.object.account.spawn_point = home
		Main.messaging:server_event("create-marker", "home", home)
	end}
