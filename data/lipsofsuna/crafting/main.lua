local CraftingUtils = require("crafting/crafting-utils")

Main.main_start_hooks:register(0, function(secs)
	Main.crafting_utils = CraftingUtils()
end)
