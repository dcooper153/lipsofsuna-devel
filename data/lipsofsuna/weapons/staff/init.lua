Program:add_path(Mod.path)

Effectspec{
	name = "staffflame1",
	node = "#flame",
	particle = "torchfx1"}

Effectspec{
	name = "staffflame2",
	node = "#flame2",
	particle = "torchfx1"}

Itemspec{
	name = "wooden staff",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default staff equipped"},
	crafting_materials = {["log"] = 5},
	damage_mining = 1,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	health = 100,
	icon = "staff1",
	influences = {["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 7,
	model = "spear1",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "luminous staff",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default staff equipped"},
	crafting_materials = {["log"] = 5, ["crimson stone"] = 2},
	damage_mining = 1,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	health = 300,
	icon = "staff2",
	influences = {["fire damage"] = 2, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 7,
	model = "staff1",
	special_effects = {"staffflame1", "staffflame2"},
	water_gravity = Vector(0,6,0)}
