Globaleventspec{
	name = "brigand camp",
	update = function(self, event, secs)
		-- Update the timer.
		event.timer = (event.timer or 0) + secs
		if event.timer < 60 then return end
		event.timer = 0
		-- Find the first spawn point.
		local spawn = Globaleventmanager:find_actor_spawn_point()
		if not spawn then return end
		-- Spawn the brigands.
		Voxel:place_actor{point = spawn:copy():multiply(Voxel.tile_scale), name = "brigandmale"}
		local count = math.random(3,6)
		for i = 1,count do
			local p = Utils:find_spawn_point(spawn:copy():add_xyz(
				5 * math.random(), 5 * math.random(), 5 * math.random()))
			if p then
				Voxel:place_actor{point = p:multiply(Voxel.tile_scale), name = "brigandmale"}
			end
		end
	end}
