Program:add_path(Mod.path)

Itemspec{
	name = "iron greaves",
	model = "legplate1",
	icon = "metalboot1",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 3},
	effect_craft = "craftmetal1",
	equipment_models = {
		["aer/android/devora/wyrm"] = {feet = false, legs = "aerlegplate1"}},
	equipment_slot = "feet",
	mass = 15}

Itemspec{
	name = "iron gauntlets",
	model = "armplate1",
	icon = "metalglove1",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 2},
	effect_craft = "craftmetal1",
	equipment_models = {
		["aer/android/devora/kraken/wyrm"] = {arms = "aerarmplate1", hands = false}},
	equipment_slot = "arms",
	mass = 10}

Itemspec{
	name = "iron breastplate",
	model = "upperplate1",
	icon = "metalarmor1",
	armor_class = 0.18,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 5},
	effect_craft = "craftmetal1",
	equipment_models = {
		["aer/android/devora/kraken/wyrm"] = {upper_safe = false, upper1 = "aerupperplate1"}},
	equipment_slot = "upperbody",
	equipment_slots_reserved = {"lowerbody"},
	mass = 25}
