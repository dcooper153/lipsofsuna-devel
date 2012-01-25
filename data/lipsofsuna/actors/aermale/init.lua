Program:add_path(Mod.path)

Itemspec{
	name = "shirt",
	model = "book-000",
	icon = "cloth1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 1},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aermale"] = {upper = "aermale1shirt1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "pants",
	icon = "cloth1",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 1},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aermale"] = {legs = "aermale1pants1"}},
	equipment_slot = "lowerbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}

Species{
	name = "aermale",
	base = "race",
	animations = {
		["land ground"] = {animation = "land", channel = 5, fade_in = 0.1, fade_out = 0.5, weight = 10, node_weights = {LOWER = 1000}},
		["spell self"] = {animation = "spell-self", channel = 2, weight = 30}},
	effect_falling_damage = "aerhurt1",
	models = {
		skeleton = "aermale1",
		arms = "aermale1arms1",
		hands = "aermale1hands1",
		head = "aermale1head1",
		legs = "aermale1legs1",
		feet = "aermale1feet1",
		eyes = "aermale1eyes1",
		upper = "aermale1chest1"},
	hair_styles = {
		{"Bald", ""},
		{"Short", "aermale1hair1"}},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 100, val = 50},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 20},
		{name = "willpower", max = 80, val = 30}},
	tilt_bone = {"back1", "back2", "back3"},
	timing_attack_throw = 4,
	timing_spell_ranged = 4,
	timing_spell_touch = 4}

-- TODO
Species{
	name = "androidmale",
	base = "android",
	equipment_class = "aermale",
	model = "aermale1",
	models = {
		skeleton = "aermale1",
		arms = "aermale1arms1",
		hands = "aermale1hands1",
		head = "aermale1head1",
		legs = "aermale1legs1",
		feet = "aermale1feet1",
		eyes = "aermale1eyes1",
		upper = "aermale1chest1"},
	hair_styles = {
		{"Short", "aermale1hair1"}}}

-- TODO
Species{
	name = "wyrmmale",
	base = "wyrm",
	equipment_class = "aermale",
	model = "aermale1",
	models = {
		skeleton = "aermale1",
		arms = "aermale1arms1",
		hands = "aermale1hands1",
		head = "aermale1head1",
		legs = "aermale1legs1",
		feet = "aermale1feet1",
		eyes = "aermale1eyes1",
		upper = "aermale1chest1"},
	hair_styles = {
		{"Short", "aermale1hair1"}}}

Species{
	name = "lipscitizenmale",
	base = "aermale",
	dialog = "lips citizen",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather chest", "leather pants", "leather boots", "leather gloves", "leather helm"}}

Species{
	name = "Mentor",
	base = "aermale",
	dialog = "mentor",
	marker = "mentor",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"shirt", "pants", "leather boots"}}

Species{
	name = "Col",
	base = "aermale",
	dialog = "col",
	marker = "col",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"shirt", "pants", "leather boots"}}

Species{
	name = "Guildmaster Sam",
	base = "aermale",
	dialog = "guildmastersam",
	marker = "guildmastersam",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"shirt", "pants", "leather boots","adamantium mattock"}}

Species{
	name = "Roland",
	base = "aermale",
	dialog = "roland",
	marker = "roland",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = {"default",255,0,0},
	factions = {"neutral"},
	hair_style = {"aermale1hair1",128,128,128},
	inventory_items = {"shirt", "pants", "leather boots"}}
