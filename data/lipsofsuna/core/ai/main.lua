local AiManager = require("core/ai/ai-manager")

Main.main_start_hooks:register(0, function(secs)
	Main.ai = AiManager()
end)
