Program:add_path(Mod.path)

Effectspec{
	name = "staffflame1",
	node = "#flame",
	particle = "torchfx1"}

Effectspec{
	name = "staffflame2",
	node = "#flame2",
	particle = "torchfx1"}

CraftingRecipeSpec{
	name = "wooden staff",
	effect = "craftwood1",
	materials = {["log"] = 4},
	modes = {["wood"] = true}}

Itemspec{
	name = "wooden staff",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default staff equipped"},
	damage_mining = 1,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	health = 100,
	icon = "staff1",
	modifiers = {["physical damage"] = 4},
	modifiers_bonus = {"melee"},
	mass = 7,
	model = "spear1",
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "luminous staff",
	effect = "craftwood1",
	materials = {["log"] = 4, ["crimson stone"] = 2},
	modes = {["wood"] = true}}

Itemspec{
	name = "luminous staff",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default staff equipped"},
	damage_mining = 1,
	equipment_anchor_rotation = {-0.325667, 0, 0, 0.945485},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	health = 300,
	icon = "staff2",
	modifiers = {["fire damage"] = 2, ["physical damage"] = 4},
	modifiers_bonus = {"melee"},
	mass = 7,
	model = "staff1",
	special_effects = {"staffflame1", "staffflame2"},
	water_gravity = Vector(0,6,0)}
