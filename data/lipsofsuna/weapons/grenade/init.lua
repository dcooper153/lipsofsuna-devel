Program:add_path(Mod.path)

AnimationProfileSpec{
	name = "default grenade equipped",
	animations = {}} --TODO

CraftingRecipeSpec{
	name = "aquanite grenade",
	effect = "craftmetal1",
	materials = {["aquanite stone"] = 1, ["iron ingot"] = 1},
	modes = {["metal"] = true}}

Itemspec{
	name = "aquanite grenade",
	categories = {["explosive"] = true, ["throwable"] = true, ["weapon"] = true},
	actions = {["right"] = "throw"},
	animations_equipped = {["default"] = "default grenade equipped"},
	damage_mining = 1,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	icon = "grenade1",
	influences = {["cold damage"] = 15, ["physical damage"] = 5},
	influences_bonus = {"explosives"},
	mass = 3,
	model = "grenade1",
	stacking = true}

CraftingRecipeSpec{
	name = "crimson grenade",
	effect = "craftmetal1",
	materials = {["crimson stone"] = 1, ["iron ingot"] = 1},
	modes = {["metal"] = true}}

Itemspec{
	name = "crimson grenade",
	categories = {["explosive"] = true, ["throwable"] = true, ["weapon"] = true},
	actions = {["right"] = "throw"},
	animations_equipped = {["default"] = "default grenade equipped"},
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
