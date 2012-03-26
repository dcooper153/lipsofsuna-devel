Program:add_path(Mod.path)

Species{
	name = "wyrm",
	base = "race",
	damage_from_magma = 0,
	equipment_class = "wyrm",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		eyes = "aereye1",
		head = "aerhead1",
		hands = "aerhand1",
		feet = "aerfoot1",
		legs = "aerleg1",
		lower = "aerlower1",
		lower_safe = "aerbloomers1",
		upper = "aerupper1",
		upper_safe = "aerbloomerstop1"},
	tilt_bone = {"back1", "back2", "back3"},
	vulnerabilities = {cold = 2, fire = 0.5}}

Species{
	name = "wyrm-player",
	base = "wyrm",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "dress", "leather leggings"}}
