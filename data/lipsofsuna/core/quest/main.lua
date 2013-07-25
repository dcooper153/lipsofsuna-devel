local QuestManager = require("core/quest/quest-manager")

Main.game_start_hooks:register(0, function()
	Main.quests = QuestManager()
end)

Main.game_end_hooks:register(0, function()
	Main.quests = nil
end)

Main.game_load_hooks:register(5, function(db)
	if not Main.quests then return end
	Main.quests:load_quests()
end)
