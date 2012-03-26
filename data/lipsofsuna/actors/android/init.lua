Program:add_path(Mod.path)

Species{
	name = "android",
	base = "race",
	damage_from_magma = 2,
	damage_from_water = 5,
	equipment_class = "aer",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		eyes = "aereye1",
		feet = "aerfoot1",
		hands = "aerhand1",
		head = "aerhead1",
		legs = "aerleg1",
		lower = "aerlower1",
		lower_safe = "aerbloomers1",
		upper = "aerupper1",
		upper_safe = "aerbloomerstop1"},
	skin_shader = "android",
	skin_textures = {"android1", "aer1n"},
	tilt_bone = {"back1", "back2", "back3"}}

Species{
	name = "android-player",
	base = "android",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "dress", "leather leggings"}}
