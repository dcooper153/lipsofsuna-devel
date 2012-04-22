Program:add_path(Mod.path)

Itemspec{
	name = "wooden staff",
	model = "spear1",
	icon = "staff1",
	animation_attack = "attack staff",
	animation_charge = "charge staff",
	animation_hold = "hold staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 5},
	damage_mining = 1,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 100,
	influences = {["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "luminous staff",
	model = "staff1",
	icon = "staff2",
	animation_attack = "attack staff",
	animation_charge = "charge staff",
	animation_hold = "hold staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 5, ["crimson stone"] = 2},
	damage_mining = 1,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 300,
	influences = {["fire damage"] = 2, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 7,
	special_effects = {
		{node = "#flame", model = "torchfx1"},
		{node = "#flame2", model = "torchfx1"}},
	water_gravity = Vector(0,6,0)}
