local Client = require("core/client/client")
local ShortcutManager = require("core/shortcut/shortcut-manager")

Client:register_init_hook(5, function()
	Main.shortcuts = ShortcutManager()
end)

Client:register_play_hook(5, function()
	Main.shortcuts:clear_all_shortcuts()
end)
