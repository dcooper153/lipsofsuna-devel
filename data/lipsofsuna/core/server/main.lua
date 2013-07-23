local Game = require("core/server/game")

Main.game_modes:register("Server", function()
	Main.game = Game("server", Main.settings.file, Main.settings.port)
	Server:init(true)
	Server:load()
	Program:set_sleep(1/60)
end)

Main.main_start_hooks:register(5, function(secs)
	Main.server = Server --FIXME
	Main.objects.object_update_hooks:register(5, function(object, secs)
		if not object:has_server_data() then return end
		-- Freeze until the terrain has been loaded.
		if not Main.terrain:is_point_loaded(object:get_position()) then
			object.physics:set_physics("static")
		else
			object.physics:set_physics(object:get_physics_mode())
		end
	end)
end)

Main.update_hooks:register(5, function(secs)
	Main.timing:start_action("server")
	Main.server:update(secs)
end)
