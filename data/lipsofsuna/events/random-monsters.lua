local MapUtils = require("core/server/map-utils")

Globaleventspec{
	name = "random monsters",
	update = function(self, event, secs)
		local spawn = Server.events:find_actor_spawn_point()
		if not spawn then return end
		local d = Utils:get_spawn_point_difficulty(spawn, true)
		MapUtils:place_actor{point = spawn, category = "enemy", difficulty = d}
	end}
