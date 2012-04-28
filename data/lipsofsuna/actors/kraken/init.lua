Program:add_path(Mod.path)

Actorspec{
	name = "kraken",
	base = "race",
	damage_from_magma = 9,
	damage_from_water = -2,
	equipment_class = "kraken",
	model = "kraken1",
	models = {
		skeleton = "kraken1",
		arms = "aerarm1",
		eyes = "aereye1",
		head = "aerhead1",
		hands = "aerhand1",
		feet = "krakenfoot1",
		legs = "krakenleg1",
		lower = "krakenlower1",
		upper = "aerupper1",
		upper_safe = "aerbloomerstop1"},
	skin_material = "animdiffnorm1",
	skin_textures = {"kraken1", "kraken1n"},
	tilt_bone = {"back1", "back2", "back3"},
	vulnerabilities = {["cold damage"] = 0.5, ["fire damage"] = 2},
	water_friction = 0}

Actorspec{
	name = "kraken-player",
	base = "kraken",
	ai_enabled = false,
	inventory_items = {["torch"] = 1, ["workbench"] = 1, ["dress"] = 1}}

Actorspec{
	name = "Lips guard mage",
	base = "kraken",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["lizard dress"] = 1, ["lizard gloves"] = 1, ["milky powder"] = 100},
	skills = {["Health lv1"] = true, ["Willpower lv4"] = true}}
