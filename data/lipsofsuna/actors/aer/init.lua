Program:add_path(Mod.path)

Effect{
	name = "aerhurt1",
	sound = "thud-000"}

Effect{
	name = "jump1",
	sound = "hop-000"}

Species{
	name = "aer",
	base = "race",
	animations = {
		["land ground"] = {animation = "land", channel = 5, fade_in = 0.1, fade_out = 0.5, weight = 10, node_weights = {LOWER = 1000}},
		["spell self"] = {animation = "spell-self", channel = 2, weight = 30}},
	effect_falling_damage = "aerhurt1",
	equipment_class = "aer",
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
	tilt_bone = {"back1", "back2", "back3"}}

Species{
	name = "aer-player",
	base = "aer",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "dress", "leather leggings"}}
