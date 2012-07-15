Globaleventspec{
	name = "random monsters",
	update = function(self, event, secs)
		local spawn = Globaleventmanager:find_actor_spawn_point()
		if not spawn then return end
		spawn:multiply(Voxel.tile_scale)
		local d = Utils:get_spawn_point_difficulty(spawn, true)
		Voxel:place_actor{point = spawn, category = "enemy", difficulty = d}
	end}
