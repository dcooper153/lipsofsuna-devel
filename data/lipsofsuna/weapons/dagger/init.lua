Program:add_path(Mod.path)

Itemspec{
	name = "adamantium dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	effect_attack_speedline = true,
	influences = {["physical damage"] = 10},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	influences = {["cold damage"] = 4, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	health = 500,
	mass = 15}

Itemspec{
	name = "crimson dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences = {["fire damage"] = 4, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 15}

Itemspec{
	name = "iron dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	influences = {["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 5}

Itemspec{
	name = "stone dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["granite stone"] = 2},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	influences = {["physical damage"] = 3},
	influences_bonus = {"melee"},
	health = 130,
	mass = 6}

Itemspec{
	name = "wooden dagger",
	model = "woodshortsword", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 2},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences = {["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 7,
	water_gravity = Vector(0,6,0)}
