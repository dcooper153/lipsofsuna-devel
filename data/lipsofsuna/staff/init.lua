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
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
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
	influences_base = {fire = -2, physical = -4},
	influences_bonus = {perception = 0.005, willpower = 0.005},
	mass = 7,
	special_effects = {
		{node = "#flame", model = "torchfx1"},
		{node = "#flame2", model = "torchfx1"}},
	water_gravity = Vector(0,6,0)}