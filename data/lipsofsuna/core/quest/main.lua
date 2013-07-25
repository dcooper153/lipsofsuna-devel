local QuestDatabase = require("core/quest/quest-database")
local QuestManager = require("core/quest/quest-manager")

Main.game_start_hooks:register(0, function()
	Main.quests = QuestManager()
end)

Main.game_end_hooks:register(0, function()
	Main.quests = nil
end)

Main.game_load_hooks:register(5, function(db)
	Main.quests = QuestDatabase(db)
	Main.quests:load_quests()
end)

Main.game_save_hooks:register(5, function(db, erase)
	if erase then
		Main.quests = QuestDatabase(db)
		Main.quests:reset()
	end
end)
