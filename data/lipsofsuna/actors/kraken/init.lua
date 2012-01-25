Program:add_path(Mod.path)

Species{
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
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 60, val = 20},
		{name = "perception", max = 40, val = 20},
		{name = "strength", max = 80, val = 30},
		{name = "willpower", max = 100, val = 50}},
	skin_shader = "animdiffnorm",
	skin_textures = {"kraken1", "kraken1n"},
	tilt_bone = {"back1", "back2", "back3"},
	vulnerabilities = {cold = 0.5, fire = 2},
	water_friction = 0}

Species{
	name = "kraken-player",
	base = "kraken",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "dress"}}

Species{
	name = "Lips guard mage",
	base = "kraken",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"lizard dress", "lizard gloves", ["milky powder"] = 100},
	skills = {
		{name = "dexterity", max = 80, val = 5},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 60, val = 40},
		{name = "perception", max = 40, val = 20},
		{name = "strength", max = 80, val = 5},
		{name = "willpower", max = 100, val = 80}}}
