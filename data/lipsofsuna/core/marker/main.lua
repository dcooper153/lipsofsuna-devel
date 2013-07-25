local MarkerManager = require("core/marker/marker-manager")

Main.game_start_hooks:register(0, function()
	Main.markers = MarkerManager()
end)

Main.game_end_hooks:register(0, function()
	Main.markers = nil
end)

Main.game_load_hooks:register(5, function(db)
	Main.markers:load(db)
end)

Main.game_save_hooks:register(5, function(db, erase)
	Main.markers:save(db, erase)
end)
