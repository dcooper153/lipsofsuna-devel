Program:add_path(Mod.path)

Actorspec{
	name = "fire elemental",
	base = "base",
	model = "lizardman2", --FIXME
	ai_combat_actions = {["move forward"] = true, ["ranged spell"] = true, ["touch spell"] = true},
	categories = {["enemy"] = true},
	difficulty = 0.5,
	effect_physical_damage = "fleshimpact1",
	feat_types = {["ranged spell"] = true, ["spell on touch"] = true},
	feat_effects = {["fire damage"] = true},
	mass = 100,
	reagentless_spells = true,
	skills = {["Health lv2"] = true, ["Willpower lv4"] = true},
	speed_walk = 10}
