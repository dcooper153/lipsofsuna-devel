Program:add_path(Mod.path)

Itemspec{
	name = "impstone",
	model = "firestone1",
	icon = "stone1",
	equipment_slot = "hand.R",
	categories = {"melee", "weapon"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	influences_base = {physical = -1},
	mass = 4,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "firestone",
	icon = "stone2",
	model = "firestone1",
	categories = {"explosive", "melee", "weapon"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	influences_base = {fire = -10, physical = -10},
	influences_bonus = {strength = 0.01},
	destroy_actions = {"explode"},
	destroy_timer = 5,
	equipment_slot = "hand.R",
	health = 1,
	mass = 4,
	special_effects = {
		{node = "#flame", model = "torchfx1"},
		{node = "#flame", type = "light", equation = {1,1,1}, diffuse = {1,0.5,0.25,1}}},
	stacking = false}

Species{
	name = "stone imp",
	base = "base",
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	ai_enable_block = false,
	ai_enable_strafe = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 0.8,
	categories = {"enemy", "mining"},
	difficulty = 0,
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"strength to physical damage"},
	equipment_slots = {
		{name = "hand.R", node = "#hand.R"},
		{name = "hand.L", node = "#hand.L"}},
	inventory_items = {"impstone"},
	inventory_size = 2,
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	skills = {
		{name = "health", max = 20, val = 20},
		{name = "strength", max = 10, val = 10}},
	vulnerabilities = {cold = 1.1, fire = 1.1, physical = 0.8}}

Species{
	name = "fire imp",
	base = "base",
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	ai_enable_block = false,
	ai_enable_strafe = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 0.8,
	categories = {"enemy", "mining"},
	difficulty = 0.2,
	equipment_slots = {
		{name = "hand.R", node = "#hand.R"},
		{name = "hand.L", node = "#hand.L"}},
	factions = {"evil"},
	feat_anims = {"bite", "explode"},
	feat_effects = {"fire damage", "strength to physical damage"},
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	inventory_items = {"firestone"},
	inventory_size = 2,
	skills = {
		{name = "health", max = 20, val = 20},
		{name = "strength", max = 10, val = 10}},
	vulnerabilities = {cold = 2, fire = 0.4, physical = 0.8}}
