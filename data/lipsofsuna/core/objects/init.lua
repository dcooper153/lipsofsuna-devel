local ObjectManager = require("core/objects/object-manager")

Main.main_start_hooks:register(0, function()
	Main.objects = ObjectManager(12, 1)
end)

Main.update_hooks:register(9, function(secs)
	Main.timing:start_action("objects")
	Main.objects:update(secs)
end)
