Program:add_path(Mod.path)

Effect{
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
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences_base = {physical = -18},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
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
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences_base = {cold = -7, physical = -7},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
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
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences_base = {fire = -7, physical = -7},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
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
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	influences_base = {physical = -10},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	equipment_slot = "hand.R",
	mass = 8}
