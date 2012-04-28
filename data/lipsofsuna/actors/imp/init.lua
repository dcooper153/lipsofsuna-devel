Program:add_path(Mod.path)

Effectspec{
	name = "impflame1",
	light = true,
	light_diffuse = {1,0.5,0.25,1},
	light_equation = {1,1,1},
	node = "#flame",
	particle = "torchfx1"}

Itemspec{
	name = "impstone",
	model = "firestone1",
	icon = "stone1",
	equipment_slot = "hand.R",
	categories = {["melee"] = true, ["weapon"] = true},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	influences = {["physical damage"] = 1},
	mass = 4,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "firestone",
	icon = "stone2",
	model = "firestone1",
	categories = {["explosive"] = true, ["melee"] = true, ["weapon"] = true},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	influences = {["fire damage"] = 10, ["physical damage"] = 10},
	influences_bonus = {"melee", "explosives"},
	destroy_actions = {"explode"},
	destroy_timer = 5,
	equipment_slot = "hand.R",
	health = 1,
	mass = 4,
	special_effects = {"impflame1"},
	stacking = false}

Actorspec{
	name = "stone imp",
	base = "base",
	animations = {["attack bite"] = "imp attack bite"},
	ai_enable_block = false,
	ai_enable_strafe = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 0.8,
	categories = {["enemy"] = true, ["mining"] = true},
	difficulty = 0,
	factions = {["evil"] = true},
	feat_anims = {["bite"] = true, ["tackle"] = true},
	feat_effects = {["physical damage"] = true},
	equipment_slots = {["hand.R"] = "#hand.R", ["hand.L"] = "#hand.L"},
	inventory_items = {["impstone"] = 1},
	inventory_size = 2,
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	skills = {["Nerf health lv2"] = true, ["Nerf combat lv2"] = true},
	vulnerabilities = {["cold damage"] = 1.1, ["fire damage"] = 1.1, ["physical damage"] = 0.8}}

Actorspec{
	name = "fire imp",
	base = "base",
	animations = {["attack bite"] = "imp attack bite"},
	ai_enable_block = false,
	ai_enable_strafe = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 0.8,
	categories = {["enemy"] = true, ["mining"] = true},
	difficulty = 0.2,
	equipment_slots = {["hand.R"] = "#hand.R", ["hand.L"] = "#hand.L"},
	factions = {["evil"] = true},
	feat_anims = {["bite"] = true, ["explode"] = true, ["tackle"] = true},
	feat_effects = {["fire damage"] = true, ["physical damage"] = true},
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	inventory_items = {["firestone"] = 1},
	inventory_size = 2,
	skills = {["Nerf health lv1"] = true, ["Nerf combat lv1"] = true},
	vulnerabilities = {["cold damage"] = 2, ["fire damage"] = 0.4, ["physical damage"] = 0.8}}
