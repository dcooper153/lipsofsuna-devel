Program:add_path(Mod.path)

Actorspec{
	name = "wyrm",
	base = "race",
	damage_from_magma = 0,
	equipment_class = "aer",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		head = "aerhead1",
		hands = "aerhand1",
		feet = "aerfoot1",
		legs = "aerleg1",
		lower = "aerlower1",
		lower_safe = "aerbloomers1",
		upper = "aerupper1",
		upper_safe = "aerbloomerstop1"},
	tilt_bone = {"back1", "back2", "back3"},
	vulnerabilities = {["cold damage"] = 2, ["fire damage"] = 0.5}}

Actorspec{
	name = "wyrm-player",
	base = "wyrm",
	ai_enabled = false,
	inventory_items = {["torch"] = 1, ["workbench"] = 1, ["dress"] = 1, ["leather leggings"] = 1}}
