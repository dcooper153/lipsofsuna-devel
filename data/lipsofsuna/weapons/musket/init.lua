Program:add_path(Mod.path)

Effect{
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
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 15, ["log"] = 2},
	effect_attack = "musket1",
	influences_base = {physical = -25},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "aquanite musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 15, ["log"] = 2},
	effect_attack = "musket1",
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	influences_base = {cold = -9, physical = -9},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	mass = 15}

Itemspec{
	name = "crimson musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 15, ["log"] = 2},
	effect_attack = "musket1",
	effect_craft = "craftmetal1",
	influences_base = {fire = -8, physical = -8},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "iron musket",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	animation_attack = "attack musket",
	animation_charge = "charge musket",
	animation_hold = "hold musket",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 5, ["log"] = 2},
	effect_attack = "musket1",
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	influences_base = {physical = -13},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	mass = 20}
