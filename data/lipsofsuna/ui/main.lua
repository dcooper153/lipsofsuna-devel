local Client = require("core/client/client")
local Graphics = require("system/graphics")
local Program = require("system/core")
local Time = require("system/time")
local Ui = require("ui/ui")
local WindowManager = require("ui/window-manager")

Main.update_hooks:register(0, function(secs)
	Main.windows = WindowManager()
end)

Main.main_start_hooks:register(110, function()
	Ui:init()
end)

Client:register_update_hook(5, function(secs)
	Main.timing:start_action("ui")
	Main.windows:update(secs)
end)
