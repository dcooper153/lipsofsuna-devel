Program:add_path(Mod.path)

Actorspec{
	name = "android",
	base = "race",
	damage_from_magma = 2,
	damage_from_water = 5,
	equipment_class = "aer",
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
	skin_textures = {"android1", "aer1n"},
	tilt_bone = {"back1", "back2", "back3"}}

Actorspec{
	name = "android-player",
	base = "android",
	ai_enabled = false,
	inventory_items = {["torch"] = 1, ["workbench"] = 1, ["dress"] = 1, ["leather leggings"] = 1}}

Actorskinspec{
	name = "Android skin 1",
	actors = {["android"] = true},
	material = "animdiffnorm1",
	textures = {"android1", "aer1n"}}
