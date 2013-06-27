Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "iron spear",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 4},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron spear",
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	categories = {["melee"] = true, ["weapon"] = true},
	effect_attack_speedline = true,
	icon = "staff1", --FIXME
	influences = {["physical damage"] = 10},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	mass = 15,
	model = "spear1"}
