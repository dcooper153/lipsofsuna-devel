local GlobalEventSpec = require("core/specs/globalevent")
local MapUtils = require("core/server/map-utils")

GlobalEventSpec{
	name = "random monsters",
	update = function(self, event, secs)
		local spawn = Server.events:find_actor_spawn_point()
		if not spawn then return end
		local d = Utils:get_spawn_point_difficulty(spawn, true)
		MapUtils:place_actor_by_category("enemy", spawn, nil, d)
	end}
