Program:add_path(Mod.path)

Itemspec{
	name = "iron spear",
	model = "spear1",
	icon = "staff1", --FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["iron"] = 4},
	effect_attack_speedline = true,
	influences_base = {physical = -10},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	mass = 15}
