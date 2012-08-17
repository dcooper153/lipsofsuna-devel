Program:add_path(Mod.path)

Effectspec{
	name = "crossbow1",
	sound = "weaponcrossbow1"}

Itemspec{
	name = "adamantium crossbow",
	model = "crossbow1",
	icon = "crossbow2",
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["adamantium"] = 9},
	effect_attack = "crossbow1",
	influences = {["physical damage"] = 15},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	timings = {["fire"] = 20}}

Itemspec{
	name = "aquanite crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["aquanite stone"] = 9},
	effect_attack = "crossbow1",
	effect_craft = "craftmetal1",
	influences = {["cold damage"] = 6, ["physical damage"] = 6},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	timings = {["fire"] = 20}}

Itemspec{
	name = "crimson crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["crimson stone"] = 9},
	effect_attack = "crossbow1",
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	influences = {["fire damage"] = 6, ["physical damage"] = 6},
	influences_bonus = {"ranged"},
	mass = 15,
	timings = {["fire"] = 20}}

Itemspec{
	name = "iron crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["iron ingot"] = 3},
	effect_attack = "crossbow1",
	effect_craft = "craftmetal1",
	influences = {["physical damage"] = 8},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	timings = {["fire"] = 20}}

Itemspec{
	name = "wooden crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default crossbow equipped"},
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["log"] = 5},
	effect_attack = "crossbow1",
	effect_craft = "craftwood1",
	influences = {["physical damage"] = 4},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 7,
	timings = {["fire"] = 20},
	water_gravity = Vector(0,6,0)}
