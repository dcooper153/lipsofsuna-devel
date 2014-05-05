local DialogManager = require("core/dialog/dialog-manager")
local TriggerManager = require("core/dialog/trigger-manager")

Main.game_start_hooks:register(0, function()
	Main.dialogs = DialogManager()
	Main.triggers = TriggerManager()
end)

Main.game_end_hooks:register(0, function()
	Main.dialogs = nil
	Main.triggers = nil
end)

Main.update_hooks:register(0, function(secs)
	if Main.triggers then
		Main.timing:start_action("triggers")
		Main.triggers:update(secs)
	end
end)
