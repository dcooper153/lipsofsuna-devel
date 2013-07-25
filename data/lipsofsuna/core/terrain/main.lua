local TerrainManager = require("core/terrain/terrain-manager")

Main.game_start_hooks:register(0, function()
	if Main.terrain then return end
	if not Main.game then return end
	Main.terrain = TerrainManager(12, 1,
		Main.game.database, Main.game.enable_unloading,
		Main.game.enable_generation, Main.game.enable_graphics)
end)

Main.game_end_hooks:register(100, function()
	if not Main.terrain then return end
	Main.terrain:unload_all()
	Main.terrain = nil
end)

Main.game_save_hooks:register(5, function(db, erase)
	if not Main.terrain then return end
	Main.terrain:save_all(db, erase)
end)

Main.update_hooks:register(0, function(secs)
	Main.timing:start_action("terrain")
	if Main.terrain then
		Main.terrain:update(tick)
	end
end)
