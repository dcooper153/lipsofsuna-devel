Program:add_path(Mod.path)

AnimationProfileSpec{
	name = "default axe equipped",
	animations = {}} --TODO

Itemspec{
	name = "adamantium hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["adamantium"] = 3},
	equipment_slot = "hand.R",
	icon = "axe3",
	influences = {hatchet = 10, ["physical damage"] = 10},
	influences_bonus = {"melee"},
	mass = 15,
	model = "axe1"}

Itemspec{
	name = "aquanite hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	icon = "axe2",
	influences = {["cold damage"] = 4, ["hatchet"] = 7, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	health = 500,
	mass = 15,
	model = "axe1"}

Itemspec{
	name = "crimson hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 500,
	icon = "axe2",
	influences = {["fire damage"] = 4, ["hatchet"] = 7, ["physical damage"] = 4},
	influences_bonus = {"melee"},
	mass = 15,
	model = "axe1"}

Itemspec{
	name = "iron hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["iron ingot"] = 2},
	damage_mining = 1,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 220,
	icon = "axe2",
	influences = {["hatchet"] = 5, ["physical damage"] = 5},
	influences_bonus = {"melee"},
	mass = 10,
	model = "axe1"}

Itemspec{
	name = "stone hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["granite stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	icon = "axe1",
	influences = {["hatchet"] = 3, ["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 9,
	model = "stoneaxe1",}

Itemspec{
	name = "wooden hatchet",
	categories = {["hatchet"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default axe equipped"},
	crafting_materials = {["log"] = 3},
	damage_mining = 1,
	effect_attack_speedline = true,
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	health = 100,
	icon = "axe1",
	influences = {["hatchet"] = 3, ["physical damage"] = 3},
	influences_bonus = {"melee"},
	mass = 7,
	model = "woodhatchet",
	water_gravity = Vector(0,6,0)}
