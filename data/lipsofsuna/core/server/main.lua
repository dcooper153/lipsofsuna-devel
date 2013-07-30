local Game = require("core/server/game")

Main.game_modes:register("Server", function()
	-- Start the game.
	Main.settings.generate = true --FIXME
	local file = Main.settings.file or "Dedicated" --FIXME: Default to new file
	Main.game = Game("server", file)
	if not Main.game:start() then
		print("ERROR: Unsupported save file.")
		Main.game = nil
		return
	end
	-- Initialize the server.
	Server:init(true)
	Program:set_sleep(1/60)
	-- Initialize networking.
	local port = Main.settings.port or Server.config.server_port
	Main.messaging:set_transmit_mode(true, false, port)
	-- Load the save file.
	if Main.settings.generate then
		Main.game:save(true)
	else
		Main.game:load()
	end
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
