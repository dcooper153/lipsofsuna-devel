local AccountDatabase = require("core/account/account-database")
local AccountManager = require("core/account/account-manager")

Main.game_start_hooks:register(0, function()
	Main.accounts = AccountManager()
end)

Main.game_end_hooks:register(0, function()
	Main.accounts = nil
end)

Main.game_load_hooks:register(0, function(db)
	Main.accounts = AccountDatabase(db)
end)

Main.game_save_hooks:register(0, function(db, erase)
	if erase then
		Main.accounts = AccountDatabase(db)
		Main.accounts:reset()
	end
	Main.accounts:save_accounts(erase)
end)
