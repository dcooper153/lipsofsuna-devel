Program:add_path(Mod.path)

Itemspec{
	name = "iron spear",
	model = "spear1",
	icon = "staff1", --FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["iron"] = 4},
	effect_attack_speedline = true,
	influences_base = {["physical damage"] = -10},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	mass = 15}
