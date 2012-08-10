Program:add_path(Mod.path)

Actorspec{
	name = "fire elemental",
	categories = {["enemy"] = true},
	ai_combat_actions = {["move forward"] = true, ["ranged spell"] = true, ["touch spell"] = true},
	animations = {["default"] = "lizardman"}, --FIXME
	difficulty = 0.5,
	effect_physical_damage = "fleshimpact1",
	feat_types = {["ranged spell"] = true, ["spell on touch"] = true},
	feat_effects = {["fire damage"] = true},
	mass = 100,
	model = "lizardman2", --FIXME
	reagentless_spells = true,
	skills = {["Health lv2"] = true, ["Willpower lv4"] = true},
	speed_walk = 10}
