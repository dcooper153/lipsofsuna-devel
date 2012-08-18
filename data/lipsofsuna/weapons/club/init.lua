Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "wooden club",
	effect = "craftwood1",
	materials = {["log"] = 4},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "wooden club",
	model = "woodenclub1",
	icon = "mace1",
	categories = {["melee"] = true, ["weapon"] = true},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 100,
	influences = {["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 7,
	water_gravity = Vector(0,6,0)}
