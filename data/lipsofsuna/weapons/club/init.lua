Program:add_path(Mod.path)

Itemspec{
	name = "wooden club",
	model = "woodenclub1",
	icon = "mace1",
	animation_attack = "attack blunt",
	animation_charge = "charge blunt",
	animation_hold = "hold blunt",
	categories = {["melee"] = true, ["weapon"] = true},
	crafting_materials = {["log"] = 4},
	damage_mining = 1,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences = {["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 7,
	water_gravity = Vector(0,6,0)}
