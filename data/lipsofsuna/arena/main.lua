local CombatUtils = require("arena/combat-utils")

Main.main_start_hooks:register(0, function(secs)
	Main.combat_utils = CombatUtils()
end)
