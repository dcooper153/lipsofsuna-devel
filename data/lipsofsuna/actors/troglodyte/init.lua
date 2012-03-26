Program:add_path(Mod.path)

Itemspec{
	name = "trog rags",
	model = "book-000",
	icon = "cloth1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {log = 1},
	effect_craft = "craftsewing1",
	equipment_models = {
		["troglodyte female"] = {upper1 = "trogfemrags1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}

Species{
	name = "troglodyte female",
	base = "race",
	ai_offense_factor = 0.9,
	animations = {
		["attack axe"] = {animation = "punch", channel = 2, fade_in = 0.1, weight = 100},
		["attack blunt"] = {animation = "punch", channel = 2, fade_in = 0.1, weight = 100},
		["attack sword"] = {animation = "punch", channel = 2, weight = 100},
		["attack staff"] = {animation = "punch", channel = 2, weight = 100},
		["hold axe"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold blunt"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold bulky"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold staff"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold shield"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5},
		["hold sword"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold torch"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5}},
	categories = {"enemy"},
	difficulty = 0.25,
	effect_physical_damage = "fleshimpact1",
	equipment_class = "troglodyte",
	eye_style = "random",
	hair_style = "random",
	factions = {"evil"},
	inventory_items = {"wooden club", "trog rags"},
	loot_categories = {"material"},
	loot_count = {2, 4},
	model = "trogfem1",
	models = {
		skeleton = "trogfem1",
		--arms = "trogfemarmnude1",
		head = "trogfemhead1",
		legs = "trogfemlegnude1",
		lower = "trogfemlowernude1",
		upper = "trogfemuppernude1"},
	hair_styles = {	
		{"troglong", "trogfemlonghair1"},
		{"trogshort", "trogfemshorthair1"}},
	skin_styles = {
		{""}}}
