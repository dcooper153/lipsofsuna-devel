Program:add_path(Mod.path)

Itemspec{
	name = "adamantium mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	description = "A high quality mattock used to mine ores",
	influences_base = {["physical damage"] = -10},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "aquanite mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	description = "A high quality mattock used to mine ores",
	crafting_materials = {["aquanite stone"] = 9},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {["cold damage"] = -3, ["physical damage"] = -4},
	influences_bonus = {"melee"},
	mass = 15}

Itemspec{
	name = "crimson mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	description = "A high quality mattock used to mine ores",
	crafting_materials = {["crimson stone"] = 9},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {["fire damage"] = -4, ["physical damage"] = -4},
	influences_bonus = {"melee"},
	mass = 15}

Itemspec{
	name = "iron mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	description = "A medium quality mattock used to mine ores",
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {["physical damage"] = -5},
	influences_bonus = {"melee"},
	mass = 15}

Itemspec{
	name = "stone mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["granite stone"] = 4},
	description = "A low quality mattock used to mine ores",
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	influences_base = {["physical damage"] = -3},
	influences_bonus = {"melee"},
	mass = 12}

Itemspec{
	name = "wooden mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["log"] = 4},
	damage_mining = 1,
	description = "A low quality mattock used to mine ores",
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {["physical damage"] = -3},
	influences_bonus = {"melee"},
	mass = 7,
	water_gravity = Vector(0,6,0)}
