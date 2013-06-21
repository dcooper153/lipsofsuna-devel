local File = require("system/file")
local ObjectManager = require("core/objects/object-manager")

Main.main_start_hooks:register(0, function()
	Main.objects = ObjectManager()
end)
