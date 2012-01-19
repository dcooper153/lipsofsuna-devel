Program:add_path(Mod.path)

Itemspec{
	name = "adamantium hatchet",
	model = "axe1",
	icon = "axe3",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	influences_base = {hatchet = -10, physical = -10},
	influences_bonus = {strength = 0.01},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite hatchet",
	model = "axe1",
	icon = "axe2",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	influences_base = {cold = -4, hatchet = -7, physical = -4},
	influences_bonus = {strength = 0.01},
	health = 500,
	mass = 15}

Itemspec{
	name = "crimson hatchet",
	model = "axe1",
	icon = "axe2",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {fire = -4, hatchet = -7, physical = -4},
	influences_bonus = {strength = 0.01},
	mass = 15}

Itemspec{
	name = "iron hatchet",
	model = "axe1",
	icon = "axe2",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 2},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {hatchet = -5, physical = -5},
	influences_bonus = {strength = 0.01},
	mass = 10}

Itemspec{
	name = "stone hatchet",
	model = "stoneaxe1",
	icon = "axe1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["granite stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	influences_base = {hatchet = -3, physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 9}

Itemspec{
	name = "wooden hatchet",
	model = "woodhatchet",
	icon = "axe1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["log"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {hatchet = -3, physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 7,
	water_gravity = Vector(0,6,0)}
