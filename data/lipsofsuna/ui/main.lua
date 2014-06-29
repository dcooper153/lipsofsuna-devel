local Client = require("core/client/client")
local Ui = require("ui/ui")
local WindowManager = require("ui/window-manager")

Main.main_start_hooks:register(1001, function(secs)
	Theme:init()
	Main.windows = WindowManager()
	Main.windows:add_window(Ui, true) -- FIXME: Hack
end)

Main.update_hooks:register(0, function(secs)
	-- TODO: Update here instead
end)

Main.main_start_hooks:register(110, function()
	Ui:init()
end)

Client:register_update_hook(5, function(secs)
	Main.timing:start_action("ui")
	Main.windows:update(secs)
end)
