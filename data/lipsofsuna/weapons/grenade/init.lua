Program:add_path(Mod.path)

AnimationProfileSpec{
	name = "default grenade equipped",
	animations = {}} --TODO

Itemspec{
	name = "aquanite grenade",
	categories = {["explosive"] = true, ["throwable"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default grenade equipped"},
	crafting_materials = {["aquanite stone"] = 1, ["iron ingot"] = 1},
	damage_mining = 1,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	effect_craft = "craftmetal1",
	equipment_slot = "hand.R",
	health = 1,
	icon = "grenade1",
	influences = {["cold damage"] = 15, ["physical damage"] = 5},
	influences_bonus = {"explosives"},
	mass = 3,
	model = "grenade1",
	stacking = true}

Itemspec{
	name = "crimson grenade",
	categories = {["explosive"] = true, ["throwable"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default grenade equipped"},
	crafting_materials = {["crimson stone"] = 1, ["iron ingot"] = 1},
	effect_craft = "craftmetal1",
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	icon = "grenade1",
	influences = {["fire damage"] = 15, ["physical damage"] = 5},
	influences_bonus = {"explosives"},
	mass = 3,
	model = "grenade1",
	stacking = true}
