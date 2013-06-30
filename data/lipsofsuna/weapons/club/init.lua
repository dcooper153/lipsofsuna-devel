Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "wooden club",
	effect = "craftwood1",
	materials = {["log"] = 4},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "wooden club",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 100,
	icon = "mace1",
	modifiers = {["physical damage"] = 4},
	modifiers_bonus = {"melee"},
	mass = 7,
	model = "woodenclub1",
	water_gravity = Vector(0,6,0)}
