Program:add_path(Mod.path)

Species{
	name = "brigand",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.5,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron sword", "leather top", "leather pants", "leather leggings"},
	loot_categories = {"armor", "material", "potion", "shield", "weapon"},
	loot_count = {2, 4},
	personality = "savage"}

Species{
	name = "brigand king",
	base = "aer",
	categories = {"special"},
	difficulty = 1,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	marker = "brigandking",
	inventory_items = {"adamantium sword", "iron breastplate", "iron greaves", "iron gauntlets"},
	loot_categories = {"armor", "shield", "weapon"},
	loot_count = {5, 10},
	skills = {"Health lv4"},
	personality = "savage"}

Species{
	name = "novice brigand archer",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.3,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"wooden crossbow", "leather top", "leather pants", "leather leggings", ["arrow"] = 20},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count = {2, 4},
	skills = {"Health lv1"},
	personality = "savage"}

Species{
	name = "master brigand archer",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.7,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron crossbow", "iron breastplate", "iron greaves", "iron gauntlets", ["arrow"] = 40},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count = {2, 4},
	skills = {"Health lv3"},
	personality = "savage"}

Species{
	name = "suicide bomber",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.5,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather top", "leather pants", ["crimson grenade"] = 15},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count = {2, 4},
	skills = {"Health lv3"},
	personality = "savage"}

Species{
	name = "brigandmale",
	base = "aermale",
	categories = {"enemy"},
	difficulty = 0.5,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather brigand chest", "leather brigand pants", "leather boots", "leather gloves", "leather helm"},
	loot_categories = {"armor", "material", "potion", "shield", "weapon"},
	loot_count = {2, 4},
	personality = "savage"}

Itemspec{
	name = "leather brigand pants",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/devora/kraken/wyrm"] = {skirt = "aerlowerarmor1", panties = "panties2"},
		["kraken"] = {skirt = "aerlowerarmor1"},
		["aermale"] = {legs = "aermale1leatherlegs1"}},
	equipment_slot = "lowerbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "leather brigand chest",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/devora/kraken/wyrm"] = {upper1 = "aerleatherchest1"},
		["aermale"] = {upper = "aermale1leatherchest1"}},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}
