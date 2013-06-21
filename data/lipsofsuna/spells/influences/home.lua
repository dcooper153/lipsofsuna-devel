Feateffectspec:extend{
	name = "home",
	touch = function(self, args)
		if not args.object then return end
		if not args.object.account then return end
		local home = args.object:get_position()
		args.object.account.spawn_point = home
		Main.messaging:server_event("create-marker", "home", home)
	end}
