Program:add_path(Mod.path)

Effectspec{
	name = "crossbow1",
	sound = "weaponcrossbow1"}

CraftingRecipeSpec{
	name = "adamantium crossbow",
	effect = "craftmetal1",
	materials = {["adamantium"] = 9},
	modes = {["metal"] = true}}

Itemspec{
	name = "adamantium crossbow",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	effect_attack = "crossbow1",
	icon = "crossbow2",
	influences = {["physical damage"] = 15},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	model = "crossbow1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "aquanite crossbow",
	effect = "craftmetal1",
	materials = {["aquanite stone"] = 9},
	modes = {["metal"] = true}}

Itemspec{
	name = "aquanite crossbow",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	effect_attack = "crossbow1",
	icon = "crossbow1",
	influences = {["cold damage"] = 6, ["physical damage"] = 6},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	model = "crossbow1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "crimson crossbow",
	effect = "craftmetal1",
	materials = {["crimson stone"] = 9},
	modes = {["metal"] = true}}

Itemspec{
	name = "crimson crossbow",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	effect_attack = "crossbow1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	icon = "crossbow1",
	influences = {["fire damage"] = 6, ["physical damage"] = 6},
	influences_bonus = {"ranged"},
	mass = 15,
	model = "crossbow1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "iron crossbow",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron crossbow",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	effect_attack = "crossbow1",
	icon = "crossbow1",
	influences = {["physical damage"] = 8},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	model = "crossbow1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "wooden crossbow",
	effect = "craftwood1",
	materials = {["log"] = 5},
	modes = {["wood"] = true}}

Itemspec{
	name = "wooden crossbow",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	effect_attack = "crossbow1",
	icon = "crossbow1",
	influences = {["physical damage"] = 4},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 7,
	model = "crossbow1",
	timings = {["fire"] = 20},
	water_gravity = Vector(0,6,0)}
