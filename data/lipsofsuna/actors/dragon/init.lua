Program:add_path(Mod.path)

Effectspec{
	name = "dragonbreath1",
	node = "head",
	particle = "firebreath1",
	sound = "squish-000"} --FIXME

Itemspec{
	name = "dragon scale",
	icon = "bone1", -- FIXME
	model = "book-000", -- FIXME
	mass = 1,
	categories = {["material"] = true},
	stacking = true}

Actorspec{
	name = "dragon",
	base = "base",
	model = "dragon1",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {["attack bite"] = "dragon attack bite", ["attack left claw"] = "dragon attack left claw", ["attack right claw"] = "dragon attack right claw", ["spell ranged"] = "dragon spell ranged"},
	difficulty = 1,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	feat_anims = {["dragon bite"] = true, ["dragon left claw"] = true, ["dragon right claw"] = true},--, ["ranged spell"] = true},
	feat_effects = {["fire damage"] = true, ["physical damage"] = true},
	inventory_size = 10,
	inventory_items = {["dragon scale"] = 5, ["adamantium"] = 4, ["crimson stone"] = 2},
	mass = 1000,
	speed_walk = 1,
	skills = {["Health lv4"] = true, ["Willpower lv2"] = true, ["Fighter"] = true},
	vulnerabilities = {["cold damage"] = 2, ["fire damage"] = 0.3}}
