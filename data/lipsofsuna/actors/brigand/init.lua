Program:add_path(Mod.path)

Actorspec{
	name = "brigand",
	base = "aer",
	categories = {["enemy"] = true},
	difficulty = 0.5,
	factions = {["evil"] = true},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["iron sword"] = 1, ["leather top"] = 1, ["leather pants"] = 1, ["leather leggings"] = 1},
	loot_categories = {"armor", "material", "potion", "shield", "weapon"},
	loot_count_min = 2,
	loot_count_max = 4,
	personality = "savage"}

Actorspec{
	name = "brigand king",
	base = "aer",
	categories = {["special"] = true},
	difficulty = 1,
	factions = {["evil"] = true},
	eye_style = "random",
	hair_style = "random",
	marker = "brigandking",
	inventory_items = {["adamantium sword"] = 1, ["iron breastplate"] = 1, ["iron greaves"] = 1, ["iron gauntlets"] = 1},
	loot_categories = {"armor", "shield", "weapon"},
	loot_count_min = 5,
	loot_count_max = 10,
	skills = {["Health lv4"] = true},
	personality = "savage"}

Actorspec{
	name = "novice brigand archer",
	base = "aer",
	categories = {["enemy"] = true},
	difficulty = 0.3,
	factions = {["evil"] = true},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["wooden crossbow"] = 1, ["leather top"] = 1, ["leather pants"] = 1, ["leather leggings"] = 1, ["arrow"] = 20},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count_min = 2,
	loot_count_max = 4,
	skills = {["Health lv1"] = true},
	personality = "savage"}

Actorspec{
	name = "master brigand archer",
	base = "aer",
	categories = {["enemy"] = true},
	difficulty = 0.7,
	factions = {["evil"] = true},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["iron crossbow"] = 1, ["iron breastplate"] = 1, ["iron greaves"] = 1, ["iron gauntlets"] = 1, ["arrow"] = 40},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count_min = 2,
	loot_count_max = 4,
	skills = {["Health lv3"] = true},
	personality = "savage"}

Actorspec{
	name = "suicide bomber",
	base = "aer",
	categories = {["enemy"] = true},
	difficulty = 0.5,
	factions = {["evil"] = true},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["leather top"] = 1, ["leather pants"] = 1, ["crimson grenade"] = 15},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count_min = 2,
	loot_count_max = 4,
	skills = {["Health lv3"] = true},
	personality = "savage"}

Actorspec{
	name = "brigandmale",
	base = "aermale",
	categories = {["enemy"] = true},
	difficulty = 0.5,
	factions = {["evil"] = true},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["leather brigand chest"] = 1, ["leather brigand pants"] = 1, ["leather boots"] = 1, ["leather gloves"] = 1, ["leather helm"] = 1},
	loot_categories = {"armor", "material", "potion", "shield", "weapon"},
	loot_count_min = 2,
	loot_count_max = 4,
	personality = "savage"}

Itemspec{
	name = "leather brigand pants",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer"] = {skirt = "aerlowerarmor1", panties = "panties2"},
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
	categories = {["armor"] = true},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer"] = {upper1 = "aerleatherchest1"},
		["kraken"] = {upper1 = "aerleatherchest1"},
		["aermale"] = {upper = "aermale1leatherchest1"}},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}
