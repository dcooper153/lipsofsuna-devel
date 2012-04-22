Program:add_path(Mod.path)

Itemspec{
	name = "iron halberd",
	model = "halberd1",
	icon = "staff1", --FIXME
	animation_attack = "attack staff",
	animation_charge = "charge staff",
	animation_hold = "hold staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron"] = 4},
	effect_attack_speedline = true,
	influences = {["physical damage"] = 10},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	mass = 15}
