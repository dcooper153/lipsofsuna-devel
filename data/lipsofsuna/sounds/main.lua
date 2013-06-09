local Client = require("core/client/client")
local Sound = require("system/sound")

-- FIXME: Most of these should be registered elsewhere.
Client:register_update_hook(15, function(secs)
	-- Sound playback.
	if Client.player_object then
		local p,r = Client.player_object:find_node{name = "#neck", space = "world"}
		if p then
			Sound:set_listener_position(p)
			Sound:set_listener_rotation(r)
		else
			Sound:set_listener_position(Client.player_object:get_position() + Vector(0,1.5,0))
			Sound:set_listener_rotation(Client.player_object:get_rotation())
		end
		local vel = Client.player_object:get_velocity()
		Sound:set_listener_velocity(vel)
	end
end)
