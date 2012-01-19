Program:add_path(Mod.path)

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
	loot_count = {2, 4}}

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
