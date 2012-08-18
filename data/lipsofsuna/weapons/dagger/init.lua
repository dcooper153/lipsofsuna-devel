Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "adamantium dagger",
	effect = "craftmetal1",
	materials = {["adamantium"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "adamantium dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {["melee"] = true, ["weapon"] = true},
	effect_attack_speedline = true,
	influences = {["physical damage"] = 10},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	mass = 15}

CraftingRecipeSpec{
	name = "aquanite dagger",
	effect = "craftmetal1",
	materials = {["aquanite stone"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "aquanite dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {["melee"] = true, ["weapon"] = true},
	damage_mining = 1,
	effect_attack_speedline = true,
	influences = {["cold damage"] = 4, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	health = 500,
	mass = 15}

CraftingRecipeSpec{
	name = "crimson dagger",
	effect = "craftmetal1",
	materials = {["crimson stone"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "crimson dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {["melee"] = true, ["weapon"] = true},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 500,
	influences = {["fire damage"] = 4, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 15}

CraftingRecipeSpec{
	name = "iron dagger",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 1},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {["melee"] = true, ["weapon"] = true},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 220,
	influences = {["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 5}

CraftingRecipeSpec{
	name = "stone dagger",
	effect = "craftmetal1",
	materials = {["granite stone"] = 2},
	modes = {["metal"] = true, ["default"] = true}}

Itemspec{
	name = "stone dagger",
	model = "dagger1", -- FIXME
	icon = "dagger1",
	categories = {["melee"] = true, ["weapon"] = true},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	influences = {["physical damage"] = 3},
	influences_bonus = {"melee"},
	health = 130,
	mass = 6}

CraftingRecipeSpec{
	name = "wooden dagger",
	effect = "craftwood1",
	materials = {["log"] = 2},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "wooden dagger",
	model = "woodshortsword", -- FIXME
	icon = "dagger1",
	categories = {["melee"] = true, ["weapon"] = true},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 100,
	influences = {["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 7,
	water_gravity = Vector(0,6,0)}
