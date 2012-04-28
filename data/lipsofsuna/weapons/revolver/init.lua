Program:add_path(Mod.path)

Effectspec{
	name = "revolver1",
	sound = "weaponmusket1"} -- TODO: Separate sound?

Itemspec{
	name = "adamantium revolver",
	model = "revolver1",
	icon = "revolver1",
	ammo_type = "bullet",
	animation_attack = "attack revolver",
	animation_charge = "charge revolver",
	animation_hold = "hold revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["adamantium"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences = {["physical damage"] = 18},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8}

Itemspec{
	name = "aquanite revolver",
	model = "revolver1",
	icon = "revolver1",
	ammo_type = "bullet",
	animation_attack = "attack revolver",
	animation_charge = "charge revolver",
	animation_hold = "hold revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["aquanite stone"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences = {["cold damage"] = 7, ["physical damage"] = 7},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8}

Itemspec{
	name = "crimson revolver",
	model = "revolver1",
	icon = "revolver1",
	ammo_type = "bullet",
	animation_attack = "attack revolver",
	animation_charge = "charge revolver",
	animation_hold = "hold revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["crimson stone"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences = {["fire damage"] = 7, ["physical damage"] = 7},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8}

Itemspec{
	name = "iron revolver",
	model = "revolver1",
	icon = "revolver1",
	ammo_type = "bullet",
	animation_attack = "attack revolver",
	animation_charge = "charge revolver",
	animation_hold = "hold revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["iron ingot"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences = {["physical damage"] = 10},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8}
