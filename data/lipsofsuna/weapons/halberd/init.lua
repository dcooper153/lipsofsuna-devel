Program:add_path(Mod.path)

AnimationProfileSpec{
	name = "default staff equipped",
	animations = {
		["hold"] = "default hold staff"}}

CraftingRecipeSpec{
	name = "iron halberd",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 4},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron halberd",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default staff equipped"},
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	icon = "staff1", --FIXME
	influences = {["physical damage"] = 10},
	influences_bonus = {"melee"},
	mass = 15,
	model = "halberd1"}
