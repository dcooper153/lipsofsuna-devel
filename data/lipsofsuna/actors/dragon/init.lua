Program:add_path(Mod.path)

Effect{
	name = "dragonbreath1",
	model = "firebreath1",
	node = "head",
	sound = "squish-000"} --FIXME

Itemspec{
	name = "dragon scale",
	icon = "bone1", -- FIXME
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Species{
	name = "dragon",
	base = "base",
	model = "dragon1",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30},
		["attack left claw"] = {animation = "claw-left", channel = 2, weight = 30},
		["attack right claw"] = {animation = "claw-right", channel = 2, weight = 30},
		["spell ranged"] = {animation = "breathe-fire", channel = 2, weight = 30}},
	difficulty = 1,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"dragon bite", "left claw", "right claw"},--, "ranged spell"},
	feat_effects = {"fire damage", "physical damage"},
	inventory_size = 10,
	inventory_items = {["dragon scale"] = 5, ["adamantium"] = 4, ["crimson stone"] = 2},
	mass = 1000,
	speed_walk = 1,
	skills = {"Health lv4", "Willpower lv2", "Fighter"},
	vulnerabilities = {["cold damage"] = 2, ["fire damage"] = 0.3}}
