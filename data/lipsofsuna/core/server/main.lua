local Game = require("core/server/game")

Main.game_modes:register("Server", function()
	Main.game = Game("server", Main.settings.file, Main.settings.port)
	if not Main.game:start() then
		print("ERROR: Unsupported save file.")
		Main.game = nil
		return
	end
	Server:init(true)
	if Main.settings.generate then
		Game:save(true)
	else
		Game:load()
	end
	Program:set_sleep(1/60)
end)

Main.main_start_hooks:register(5, function(secs)
	Main.server = Server --FIXME
end)

Main.game_save_hooks:register(0, function(db, erase)
	if not Server.initialized then return end
	if erase then
		Main.server.object_database:clear_objects()
	end
	Main.game.sectors:save_world(erase)
	if not erase then
		Main.server.object_database:update_world_decay()
		Main.server.object_database:clear_unused_objects()
	end
	Main.accounts:save_accounts(erase)
end)

Main.update_hooks:register(5, function(secs)
	Main.timing:start_action("server")
	Main.server:update(secs)
end)
