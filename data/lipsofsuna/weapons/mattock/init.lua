Program:add_path(Mod.path)

Itemspec{
	name = "adamantium mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["adamantium"] = 9},
	description = "A high quality mattock used to mine ores",
	icon = "mattock1",
	influences = {["physical damage"] = 10},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	mass = 15,
	model = "mattock-000"}

Itemspec{
	name = "aquanite mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	description = "A high quality mattock used to mine ores",
	crafting_materials = {["aquanite stone"] = 9},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	icon = "mattock1",
	influences = {["cold damage"] = 3, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 15,
	model = "mattock-000"}

Itemspec{
	name = "crimson mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	description = "A high quality mattock used to mine ores",
	crafting_materials = {["crimson stone"] = 9},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	icon = "mattock1",
	influences = {["fire damage"] = 4, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 15,
	model = "mattock-000"}

Itemspec{
	name = "iron mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["iron ingot"] = 3},
	description = "A medium quality mattock used to mine ores",
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	icon = "mattock1",
	influences = {["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 15,
	model = "mattock-000"}

Itemspec{
	name = "stone mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["granite stone"] = 4},
	description = "A low quality mattock used to mine ores",
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	icon = "mattock1",
	influences = {["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 12,
	model = "mattock-000"}

Itemspec{
	name = "wooden mattock",
	categories = {["mattock"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["log"] = 4},
	damage_mining = 1,
	description = "A low quality mattock used to mine ores",
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	icon = "mattock1",
	influences = {["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 7,
	model = "mattock-000",
	water_gravity = Vector(0,6,0)}
