Program:add_path(Mod.path)

Itemspec{
	name = "lizard boots",
	armor_class = 0.05,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/devora/wyrm"] = {feet = "aerlizardboots1"}},
	equipment_slot = "feet",
	icon = "lizardboot1",
	mass = 4,
	model = "lizardboots1",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "lizard dress",
	armor_class = 0.1,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 6},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/devora/wyrm"] = {dress1 = "aerlizarddress1", panties = "panties2", upper_safe = false},
		["kraken"] = {dress1 = "aerlizarddress1"}},
	equipment_slot = "upperbody",
	icon = "lizardarmor1",
	mass = 7,
	model = "lizarddress1",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "lizard gloves",
	armor_class = 0.05,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/devora/kraken/wyrm"] = {hands = "aerlizardgloves1"}},
	equipment_slot = "arms",
	icon = "lizardglove1",
	mass = 4,
	model = "lizardgloves1",
	water_gravity = Vector(0,6,0)}
