Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "iron greaves",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron greaves",
	model = "legplate1",
	icon = "metalboot1",
	armor_class = 0.08,
	categories = {["armor"] = true},
	equipment_models = {
		["aer"] = {feet = "", legs = "aerlegplate1"}},
	equipment_slot = "feet",
	mass = 15}

CraftingRecipeSpec{
	name = "iron gauntlets",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 2},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron gauntlets",
	model = "armplate1",
	icon = "metalglove1",
	armor_class = 0.08,
	categories = {["armor"] = true},
	equipment_models = {
		["aer"] = {arms = "aerarmplate1", hands = ""},
		["kraken"] = {arms = "aerarmplate1", hands = ""}},
	equipment_slot = "arms",
	mass = 10}

CraftingRecipeSpec{
	name = "iron breastplate",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 5},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron breastplate",
	model = "upperplate1",
	icon = "metalarmor1",
	armor_class = 0.18,
	categories = {["armor"] = true},
	equipment_models = {
		["aer"] = {upper_safe = "", upper1 = "aerupperplate1"},
		["kraken"] = {upper_safe = "", upper1 = "aerupperplate1"}},
	equipment_slot = "upperbody",
	equipment_slots_reserved = {["lowerbody"] = true},
	mass = 25}
