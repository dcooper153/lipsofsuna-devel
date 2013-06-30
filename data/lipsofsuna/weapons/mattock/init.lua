Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "adamantium mattock",
	effect = "craftmetal1",
	materials = {["adamantium"] = 9},
	modes = {["metal"] = true}}

Itemspec{
	name = "adamantium mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	description = "A high quality mattock used to mine ores",
	icon = "mattock1",
	modifiers = {["physical damage"] = 10},
	modifiers_bonus = {"melee"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	model = "mattock-000"}

CraftingRecipeSpec{
	name = "aquanite mattock",
	effect = "craftmetal1",
	materials = {["aquanite stone"] = 9},
	modes = {["metal"] = true}}

Itemspec{
	name = "aquanite mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	description = "A high quality mattock used to mine ores",
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 500,
	icon = "mattock1",
	modifiers = {["cold damage"] = 3, ["physical damage"] = 4},
	modifiers_bonus = {"melee"},
	mass = 15,
	model = "mattock-000"}

CraftingRecipeSpec{
	name = "crimson mattock",
	effect = "craftmetal1",
	materials = {["crimson stone"] = 9},
	modes = {["metal"] = true}}

Itemspec{
	name = "crimson mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	description = "A high quality mattock used to mine ores",
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 500,
	icon = "mattock1",
	modifiers = {["fire damage"] = 4, ["physical damage"] = 4},
	modifiers_bonus = {"melee"},
	mass = 15,
	model = "mattock-000"}

CraftingRecipeSpec{
	name = "iron mattock",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 3},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	description = "A medium quality mattock used to mine ores",
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 220,
	icon = "mattock1",
	modifiers = {["physical damage"] = 5},
	modifiers_bonus = {"melee"},
	mass = 15,
	model = "mattock-000"}

CraftingRecipeSpec{
	name = "stone mattock",
	effect = "craftmetal1",
	materials = {["granite stone"] = 4},
	modes = {["metal"] = true, ["default"] = true}}

Itemspec{
	name = "stone mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	description = "A low quality mattock used to mine ores",
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	icon = "mattock1",
	modifiers = {["physical damage"] = 3},
	modifiers_bonus = {"melee"},
	mass = 12,
	model = "mattock-000"}

CraftingRecipeSpec{
	name = "wooden mattock",
	effect = "craftwood1",
	materials = {["log"] = 4},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "wooden mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	actions = {["right"] = "attack", ["left"] = "block weapon"},
	animations_equipped = {["default"] = "default axe equipped"},
	damage_mining = 1,
	description = "A low quality mattock used to mine ores",
	equipment_slot = "hand.R",
	health = 100,
	icon = "mattock1",
	modifiers = {["physical damage"] = 3},
	modifiers_bonus = {"melee"},
	mass = 7,
	model = "mattock-000",
	water_gravity = Vector(0,6,0)}
