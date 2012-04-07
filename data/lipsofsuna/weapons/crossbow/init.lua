Program:add_path(Mod.path)

Effect{
	name = "crossbow1",
	sound = "weaponcrossbow1"}

Itemspec{
	name = "adamantium crossbow",
	model = "crossbow1",
	icon = "crossbow2",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	effect_attack = "crossbow1",
	influences_base = {["physical damage"] = -15},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "aquanite crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	effect_attack = "crossbow1",
	effect_craft = "craftmetal1",
	influences_base = {["cold damage"] = -6, ["physical damage"] = -6},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "crimson crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	effect_attack = "crossbow1",
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	influences_base = {["fire damage"] = -6, ["physical damage"] = -6},
	influences_bonus = {"ranged"},
	mass = 15}

Itemspec{
	name = "iron crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	effect_attack = "crossbow1",
	effect_craft = "craftmetal1",
	influences_base = {["physical damage"] = -8},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "wooden crossbow",
	model = "crossbow1",
	icon = "crossbow1",
	ammo_type = "arrow",
	animation_attack = "attack crossbow",
	animation_charge = "charge crossbow",
	animation_hold = "hold crossbow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 5},
	effect_attack = "crossbow1",
	effect_craft = "craftwood1",
	influences_base = {["physical damage"] = -4},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,6,0),
	mass = 7}
