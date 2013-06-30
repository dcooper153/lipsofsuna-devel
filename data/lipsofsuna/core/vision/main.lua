local VisionManager = require("core/vision/vision-manager")

Main.main_start_hooks:register(5, function(secs)
	Main.vision = VisionManager(Main.objects)
end)
