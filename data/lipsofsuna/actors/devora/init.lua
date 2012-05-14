Program:add_path(Mod.path)

Actorspec{
	name = "devora",
	base = "race",
	aim_ray_center = Vector(0, 1, 0),
	body_scale_min = 0.65,
	body_scale_max = 0.8,
	camera_center = Vector(0, 1, 0),
	damage_from_magma = 9,
	equipment_class = "aer",
	hair_styles = {["Bald"] = "", ["Plant 1"] = "devorahair1", ["Plant 2"] = "devorahair2"},
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		feet = "aerfoot1",
		hands = "aerhand1",
		head = "aerhead1",
		legs = "aerleg1",
		lower = "aerlower1",
		lower_safe = "aerbloomers1",
		upper = "aerupper1",
		upper_safe = "aerbloomerstop1"},
	skin_material = "animdiffnorm1",
	skin_textures = {"devora1", "devora1n"},
	tilt_bone = {"back1", "back2", "back3"},
	vulnerabilities = {["fire damage"] = 2, ["physical damage"] = 0.8}}

Actorspec{
	name = "devora-player",
	base = "devora",
	ai_enabled = false,
	inventory_items = {["torch"] = 1, ["workbench"] = 1, ["dress"] = 1, ["leather leggings"] = 1}}

Actorspec{
	name = "feral devora",
	base = "devora",
	ai_offense_factor = 0.8,
	categories = {["enemy"] = true},
	difficulty = 0.5,
	factions = {["evil"] = true},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["dress"] = 1},
	loot_categories = {"material"},
	loot_count_min = 2,
	loot_count_max = 4}
