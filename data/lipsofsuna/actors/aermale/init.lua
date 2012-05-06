Program:add_path(Mod.path)

Itemspec{
	name = "shirt",
	model = "book-000",
	icon = "cloth1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	crafting_materials = {hide = 1},
	effect_craft = "craftsewing1",
	equipment_models = {["aermale"] = {upper = "aermale1shirt1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "pants",
	icon = "cloth1",
	model = "book-000",
	armor_class = 0.04,
	categories = {["armor"] = true},
	crafting_materials = {hide = 1},
	effect_craft = "craftsewing1",
	equipment_models = {["aermale"] = {legs = "aermale1pants1"}},
	equipment_slot = "lowerbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}

Actorspec{
	name = "aermale",
	base = "race",
	animations = {["land ground"] = "aermale land ground", ["spell self"] = "aermale spell self"},
	effect_falling_damage = "aerhurt1",
	equipment_class = "aermale",
	model = "aermale1",
	models = {
		skeleton = "aermale1",
		arms = "aermale1arms1",
		--hands = "aermale1hands1",
		head = "aermale1head1",
		legs = "aermale1legs1",
		feet = "aermale1feet1",
		eyes = "aermale1eyes1",
		upper = "aermale1chest1"},
	hair_styles = {["Bald"] = "", ["Short"] = "aermale1hair1"},
	tilt_bone = {"back1", "back2", "back3"},
	timing_attack_throw = 4,
	timing_spell_ranged = 4,
	timing_spell_touch = 4}

-- TODO
Actorspec{
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
	hair_styles = {["Short"] = "aermale1hair1"}}

-- TODO
Actorspec{
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
	hair_styles = {["Short"] = "aermale1hair1"}}

Actorspec{
	name = "lipscitizenmale",
	base = "aermale",
	dialog = "lips citizen",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["leather chest"] = 1, ["leather pants"] = 1, ["leather boots"] = 1, ["leather gloves"] = 1, ["leather helm"] = 1}}
