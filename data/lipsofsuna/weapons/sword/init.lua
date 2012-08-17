Program:add_path(Mod.path)

Itemspec{
	name = "adamantium sword",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default sword equipped"},
	crafting_materials = {["adamantium"] = 3},
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	icon = "sword3",
	influences = {["physical damage"] = 20},
	influences_bonus = {"melee"},
	mass = 15,
	model = "sword6"}

Itemspec{
	name = "aquanite sword",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default sword equipped"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	icon = "sword2",
	influences = {["cold damage"] = 8, ["physical damage"] = 8},
	influences_bonus = {"melee"},
	mass = 15,
	model = "sword5"}

Itemspec{
	name = "crimson sword",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default sword equipped"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	icon = "sword2",
	influences = {["fire damage"] = 8, ["physical damage"] = 8},
	influences_bonus = {"melee"},
	mass = 15,
	model = "sword5"}

Itemspec{
	name = "iron sword",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default sword equipped"},
	crafting_materials = {["iron ingot"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	icon = "sword2",
	influences = {["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 15,
	model = "sword1"}

Itemspec{
	name = "stone sword",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default sword equipped"},
	crafting_materials = {["granite stone"] = 5},
	damage_mining = 1,
	effect_attack_speedline = true,
	equipment_slot = "hand.R",
	health = 130,
	icon = "sword1",
	influences = {["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 15,
	model = "sword1"}

Itemspec{
	name = "wooden sword",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default sword equipped"},
	crafting_materials = {["log"] = 5},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	icon = "sword1",
	influences = {["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 7,
	model = "woodsword",
	water_gravity = Vector(0,6,0)}
