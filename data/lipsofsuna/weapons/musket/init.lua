Program:add_path(Mod.path)

Effectspec{
	name = "musket1",
	sound = "weaponmusket1"}

Itemspec{
	name = "adamantium musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["adamantium"] = 15, ["log"] = 2},
	effect_attack = "musket1",
	influences = {["physical damage"] = 25},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15}

Itemspec{
	name = "aquanite musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["aquanite stone"] = 15, ["log"] = 2},
	effect_attack = "musket1",
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	influences = {["cold damage"] = 9, ["physical damage"] = 9},
	influences_bonus = {"ranged", "guns"},
	mass = 15}

Itemspec{
	name = "crimson musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["crimson stone"] = 15, ["log"] = 2},
	effect_attack = "musket1",
	effect_craft = "craftmetal1",
	influences = {["fire damage"] = 8, ["physical damage"] = 8},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15}

Itemspec{
	name = "iron musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["iron ingot"] = 5, ["log"] = 2},
	effect_attack = "musket1",
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	influences = {["physical damage"] = 13},
	influences_bonus = {"ranged", "guns"},
	mass = 20}
