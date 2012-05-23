Program:add_path(Mod.path)

Itemspec{
	name = "trog rags",
	model = "book-000",
	icon = "cloth1",
	armor_class = 0.02,
	categories = {["armor"] = true},
	crafting_materials = {["log"] = 1},
	effect_craft = "craftsewing1",
	equipment_models = {["troglodyte"] = {upper1 = "trogfemrags1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}

Actorspec{
	name = "troglodyte female",
	base = "race",
	ai_offense_factor = 0.9,
	body_scale_min = 0.7,
	body_scale_max = 0.8,
	categories = {["enemy"] = true},
	difficulty = 0.25,
	effect_physical_damage = "fleshimpact1",
	equipment_class = "troglodyte",
	eye_style = "random",
	factions = {["evil"] = true},
	hair_style = "random",
	hair_styles = {["troglong"] = "trogfemlonghair1", ["trogshort"] = "trogfemshorthair1"},
	inventory_items = {["wooden club"] = 1, ["trog rags"] = 1},
	loot_categories = {"material"},
	loot_count_min = 2,
	loot_count_max = 4,
	model = "trogfem1",
	models = {
		skeleton = "trogfem1",
		body = "trogfembody1"},
	tilt_bone = {"back1", "back2", "back3"}}
