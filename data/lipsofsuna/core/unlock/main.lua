local UnlockManager = require("core/unlock/unlock-manager")

Main.main_start_hooks:register(5, function(secs)
	Main.unlocks = UnlockManager()
	Main.unlocks:set_messaging(true)
end)

Main.game_start_hooks:register(0, function()
	Main.unlocks:reset()
end)

Main.game_load_hooks:register(0, function(db)
	Main.unlocks:set_database(db)
	Main.unlocks:reset()
	Main.unlocks:load()
end)

Main.game_save_hooks:register(0, function(db, erase)
	Main.unlocks:set_database(db)
	if erase then
		Main.unlocks:reset()
		Main.unlocks:reset_database()
		Main.unlocks:unlock("skill", "Health lv1")
		Main.unlocks:unlock("skill", "Willpower lv1")
		Main.unlocks:unlock("action", "ranged spell")
		Main.unlocks:unlock("action", "self spell")
		Main.unlocks:unlock("modifier", "fire damage")
		Main.unlocks:unlock("modifier", "light")
		Main.unlocks:unlock("modifier", "physical damage")
		Main.unlocks:unlock("modifier", "restore health")
	end
	Main.unlocks:save()
end)
