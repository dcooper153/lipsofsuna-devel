local File = require("system/file")

File:require_directory(Mod.path, "init")

Main.main_start_hooks:register(0, function()
	Actionspec:validate_all()
	Actorspec:validate_all()
	Actorpresetspec:validate_all()
	Animationspec:validate_all()
	AnimationProfileSpec:validate_all()
	CraftingRecipeSpec:validate_all()
	Dialogspec:validate_all()
	Factionspec:validate_all()
	Feateffectspec:validate_all()
	Iconspec:validate_all()
	Itemspec:validate_all()
	Obstaclespec:validate_all()
	Patternspec:validate_all()
	Questspec:validate_all()
	Skillspec:validate_all()
	Spellspec:validate_all()
	Staticspec:validate_all()
end)

Main.main_start_hooks:register(1000, function()
	for k,v in pairs(Actorspec.dict_id) do
		v:calculate_abilities()
	end
end)
