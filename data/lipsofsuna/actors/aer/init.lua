Program:add_path(Mod.path)

Effectspec{
	name = "aerhurt1",
	sound = "thud-000"}

Effectspec{
	name = "jump1",
	sound = "hop-000"}

Actorspec{
	name = "aer",
	base = "race",
	animations = {["land ground"] = "aer land ground", ["spell self"] = "aer spell self"},
	effect_falling_damage = "aerhurt1",
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
	tilt_bone = {"back1", "back2", "back3"}}

Actorspec{
	name = "aer-player",
	base = "aer",
	ai_enabled = false,
	inventory_items = {["torch"] = 1, ["workbench"] = 1, ["dress"] = 1, ["leather leggings"] = 1}}
