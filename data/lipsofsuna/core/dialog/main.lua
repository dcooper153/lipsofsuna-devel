local DialogManager = require("core/dialog/dialog-manager")

Main.game_start_hooks:register(0, function()
	Main.dialogs = DialogManager()
end)

Main.game_end_hooks:register(0, function()
	Main.dialogs = nil
end)
