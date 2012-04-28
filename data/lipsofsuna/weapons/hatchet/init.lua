Program:add_path(Mod.path)

Itemspec{
	name = "adamantium hatchet",
	model = "axe1",
	icon = "axe3",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	crafting_materials = {["adamantium"] = 3},
	influences = {hatchet = 10, ["physical damage"] = 10},
	influences_bonus = {"melee"},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite hatchet",
	model = "axe1",
	icon = "axe2",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	influences = {["cold damage"] = 4, ["hatchet"] = 7, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	health = 500,
	mass = 15}

Itemspec{
	name = "crimson hatchet",
	model = "axe1",
	icon = "axe2",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	influences = {["fire damage"] = 4, ["hatchet"] = 7, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 15}

Itemspec{
	name = "iron hatchet",
	model = "axe1",
	icon = "axe2",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	crafting_materials = {["iron ingot"] = 2},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	influences = {["hatchet"] = 5, ["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 10}

Itemspec{
	name = "stone hatchet",
	model = "stoneaxe1",
	icon = "axe1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	crafting_materials = {["granite stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	influences = {["hatchet"] = 3, ["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 9}

Itemspec{
	name = "wooden hatchet",
	model = "woodhatchet",
	icon = "axe1",
	animation_attack = "attack axe",
	animation_charge = "charge axe",
	animation_hold = "hold axe",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	crafting_materials = {["log"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	influences = {["hatchet"] = 3, ["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 7,
	water_gravity = Vector(0,6,0)}
