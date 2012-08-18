Program:add_path(Mod.path)

Effectspec{
	name = "musket1",
	sound = "weaponmusket1"}

CraftingRecipeSpec{
	name = "adamantium musket",
	effect = "craftmetal1",
	materials = {["adamantium"] = 15, ["log"] = 2},
	modes = {["metal"] = true}}

Itemspec{
	name = "adamantium musket",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default musket equipped"},
	icon = "musket1",
	influences = {["physical damage"] = 25},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	model = "musket1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "aquanite musket",
	effect = "craftmetal1",
	materials = {["aquanite stone"] = 15, ["log"] = 2},
	modes = {["metal"] = true}}

Itemspec{
	name = "aquanite musket",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default musket equipped"},
	effect_attack = "musket1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	icon = "musket1",
	influences = {["cold damage"] = 9, ["physical damage"] = 9},
	influences_bonus = {"ranged", "guns"},
	mass = 15,
	model = "musket1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "crimson musket",
	effect = "craftmetal1",
	materials = {["crimson stone"] = 15, ["log"] = 2},
	modes = {["metal"] = true}}

Itemspec{
	name = "crimson musket",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default musket equipped"},
	effect_attack = "musket1",
	icon = "musket1",
	influences = {["fire damage"] = 8, ["physical damage"] = 8},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	model = "musket1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "iron musket",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 5, ["log"] = 2},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron musket",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default musket equipped"},
	effect_attack = "musket1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	icon = "musket1",
	influences = {["physical damage"] = 13},
	influences_bonus = {"ranged", "guns"},
	mass = 20,
	model = "musket1",
	timings = {["fire"] = 20}}
