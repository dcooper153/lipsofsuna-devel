Program:add_path(Mod.path)

Itemspec{
	name = "lizard scale",
	icon = "bone1", -- FIXME
	model = "book-000", -- FIXME
	mass = 1,
	categories = {["material"] = true},
	stacking = true}

Actorspec{
	name = "lizardman",
	base = "base",
	model = "lizardman2",
	ai_enable_block = false,
	ai_offense_factor = 0.9,
	animations = {["attack bite"] = "lizardman attack bite"},
	categories = {["enemy"] = true},
	difficulty = 0.3,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	feat_anims = {["right hand"] = true, ["bite"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 5,
	inventory_items = {["wooden club"] = 1, ["lizard scale"] = 3},
	loot_categories = {"material"},
	loot_count_min = 2,
	loot_count_max = 4,
	mass = 100,
	skills = {["Nerf health lv1"] = true},
	speed_walk = 3,
	equipment_slots = {["hand.L"] = "#hand.L", ["hand.R"] = "#hand.R"}}
