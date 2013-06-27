Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "adamantium dagger",
	effect = "craftmetal1",
	materials = {["adamantium"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "adamantium dagger",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	effect_attack_speedline = true,
	icon = "dagger1",
	influences = {["physical damage"] = 10},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	mass = 15,
	model = "dagger1"} -- FIXME

CraftingRecipeSpec{
	name = "aquanite dagger",
	effect = "craftmetal1",
	materials = {["aquanite stone"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "aquanite dagger",
	model = "dagger1", -- FIXME
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 500,
	icon = "dagger1",
	influences = {["cold damage"] = 4, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 15}

CraftingRecipeSpec{
	name = "crimson dagger",
	effect = "craftmetal1",
	materials = {["crimson stone"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "crimson dagger",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 500,
	icon = "dagger1",
	influences = {["fire damage"] = 4, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 15,
	model = "dagger1"} -- FIXME

CraftingRecipeSpec{
	name = "iron dagger",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 1},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron dagger",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 220,
	icon = "dagger1",
	influences = {["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 5,
	model = "dagger1"} -- FIXME

CraftingRecipeSpec{
	name = "stone dagger",
	effect = "craftmetal1",
	materials = {["granite stone"] = 2},
	modes = {["metal"] = true, ["default"] = true}}

Itemspec{
	name = "stone dagger",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	icon = "dagger1",
	influences = {["physical damage"] = 3},
	influences_bonus = {"melee"},
	health = 130,
	mass = 6,
	model = "dagger1"} -- FIXME

CraftingRecipeSpec{
	name = "wooden dagger",
	effect = "craftwood1",
	materials = {["log"] = 2},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "wooden dagger",
	categories = {["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 100,
	icon = "dagger1",
	influences = {["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 7,
	model = "woodshortsword", -- FIXME
	water_gravity = Vector(0,6,0)}
