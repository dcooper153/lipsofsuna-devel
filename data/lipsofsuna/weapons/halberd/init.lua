Program:add_path(Mod.path)

AnimationProfileSpec{
	name = "default staff equipped",
	animations = {
		["hold"] = "default hold staff"}}

Itemspec{
	name = "iron halberd",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default staff equipped"},
	crafting_materials = {["iron ingot"] = 4},
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	icon = "staff1", --FIXME
	influences = {["physical damage"] = 10},
	influences_bonus = {"melee"},
	mass = 15,
	model = "halberd1"}
