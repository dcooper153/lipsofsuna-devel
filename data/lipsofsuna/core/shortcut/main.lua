local Client = require("core/client/client")
local ShortcutManager = require("core/shortcut/shortcut-manager")

Main.main_start_hooks:register(110, function()
	Main.shortcuts = ShortcutManager()
end)

Client:register_play_hook(5, function()
	Main.shortcuts:clear_all_shortcuts()
end)

Main.game_start_hooks:register(10, function()
	Main.accounts.serializer:add_field("shortcuts", "lua")
	Main.accounts.login_hooks:register(10, function(account)
		if not account.shortcuts then return end
		Main.messaging:server_event("shortcut", account.client, account.shortcuts)
	end)
end)

Main.game_load_hooks:register(10, function(db)
	Main.accounts.serializer:add_field("shortcuts", "lua")
	Main.accounts.login_hooks:register(10, function(account)
		if not account.shortcuts then return end
		Main.messaging:server_event("shortcut", account.client, account.shortcuts)
	end)
end)

Main.game_save_hooks:register(10, function(db, erase)
	if not erase then return end
	Main.accounts.serializer:add_field("shortcuts", "lua")
	Main.accounts.login_hooks:register(10, function(account)
		if not account.shortcuts then return end
		Main.messaging:server_event("shortcut", account.client, account.shortcuts)
	end)
end)
