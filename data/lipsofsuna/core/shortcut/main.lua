local Client = require("core/client/client")
local ShortcutManager = require("core/shortcut/shortcut-manager")

Client:register_init_hook(5, function()
	Main.shortcuts = ShortcutManager()
end)

Client:register_play_hook(5, function()
	Main.shortcuts:clear_all_shortcuts()
end)

Main.game_start_hooks:register(10, function()
	if not Main.server then return end
	if not Main.server.account_database then return end
	Main.server.account_database.serializer:add_field("shortcuts", "lua")
	Main.server.login_hooks:register(10, function(account)
		if not account.shortcuts then return end
		Main.messaging:server_event("shortcut", account.client, account.shortcuts)
	end)
end)
