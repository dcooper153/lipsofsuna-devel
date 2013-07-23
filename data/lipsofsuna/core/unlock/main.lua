local UnlockManager = require("core/unlock/unlock-manager")

Main.main_start_hooks:register(5, function(secs)
	Main.unlocks = UnlockManager()
end)

Main.game_start_hooks:register(15, function()
	Main.unlocks:reset()
	Main.unlocks:set_database(Game.database)
	if Settings.generate then
		Main.unlocks:reset_database()
		Main.unlocks:save()
	else
		Main.unlocks:load()
	end
	Main.unlocks:unlock("skill", "Health lv1")
	Main.unlocks:unlock("skill", "Willpower lv1")
	Main.unlocks:unlock("action", "ranged spell")
	Main.unlocks:unlock("action", "self spell")
	Main.unlocks:unlock("modifier", "fire damage")
	Main.unlocks:unlock("modifier", "light")
	Main.unlocks:unlock("modifier", "physical damage")
	Main.unlocks:unlock("modifier", "restore health")
end)
