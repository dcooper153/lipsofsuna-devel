Feateffectspec:extend{
	name = "home",
	touch = function(self, args)
		if not args.object then return end
		if not args.object.account then return end
		local home = args.object.position
		args.object.account.spawn_point = home
		args.object:send(Packet(packets.MARKER_ADD, "string", "home",
			"float", home.x, "float", home.y, "float", home.z))
	end}
