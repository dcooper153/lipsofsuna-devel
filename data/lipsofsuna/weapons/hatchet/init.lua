Program:add_path(Mod.path)

AnimationProfileSpec{
	name = "default axe equipped",
	animations = {}} --TODO

CraftingRecipeSpec{
	name = "adamantium hatchet",
	effect = "craftmetal1",
	materials = {["adamantium"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "adamantium hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "right melee", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	equipment_slot = "hand.R",
	icon = "axe3",
	influences = {hatchet = 10, ["physical damage"] = 10},
	influences_bonus = {"melee"},
	mass = 15,
	model = "axe1"}

CraftingRecipeSpec{
	name = "aquanite hatchet",
	effect = "craftmetal1",
	materials = {["aquanite stone"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "aquanite hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "right melee", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	damage_mining = 1,
	equipment_slot = "hand.R",
	icon = "axe2",
	influences = {["cold damage"] = 4, ["hatchet"] = 7, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	health = 500,
	mass = 15,
	model = "axe1"}

CraftingRecipeSpec{
	name = "crimson hatchet",
	effect = "craftmetal1",
	materials = {["crimson stone"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "crimson hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "right melee", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 500,
	icon = "axe2",
	influences = {["fire damage"] = 4, ["hatchet"] = 7, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 15,
	model = "axe1"}

CraftingRecipeSpec{
	name = "iron hatchet",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 2},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "right melee", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 220,
	icon = "axe2",
	influences = {["hatchet"] = 5, ["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 10,
	model = "axe1"}

CraftingRecipeSpec{
	name = "stone hatchet",
	effect = "craftmetal1",
	materials = {["granite stone"] = 3},
	modes = {["metal"] = true, ["default"] = true}}

Itemspec{
	name = "stone hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "right melee", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	icon = "axe1",
	influences = {["hatchet"] = 3, ["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 9,
	model = "stoneaxe1",}

CraftingRecipeSpec{
	name = "wooden hatchet",
	effect = "craftwood1",
	materials = {["log"] = 3},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "wooden hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "right melee", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 100,
	icon = "axe1",
	influences = {["hatchet"] = 3, ["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 7,
	model = "woodhatchet",
	water_gravity = Vector(0,6,0)}
