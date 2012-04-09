Program:add_path(Mod.path)

Itemspec{
	name = "lizard scale",
	icon = "bone1", -- FIXME
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Species{
	name = "lizardman",
	base = "base",
	model = "lizardman2",
	ai_enable_block = false,
	ai_offense_factor = 0.9,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy"},
	difficulty = 0.3,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"right hand","bite"},
	feat_effects = {"physical damage"},
	inventory_size = 5,
	inventory_items = {"wooden club", ["lizard scale"] = 3},
	loot_categories = {"material"},
	loot_count = {2, 4},
	mass = 100,
	skills = {"Health nerf lv1"},
	speed_walk = 3,
	equipment_slots = {
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"}}}
