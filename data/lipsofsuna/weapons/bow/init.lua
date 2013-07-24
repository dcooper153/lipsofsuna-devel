Program:add_path(Mod.path)

Effectspec{
	name = "firearrow1",
	node = "#root",
	light = true,
	light_diffuse = {1,1,1,1},
	light_equation = {1,0.1,0.01}}

CraftingRecipeSpec{
	name = "arrow",
	count = 10,
	effect = "craftwood1",
	materials = {["log"] = 1},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "arrow",
	categories = {["arrow"] = true},
	modifiers = {["physical damage"] = 1},
	gravity_projectile = Vector(0,-1,0),
	icon = "arrow1",
	mass = 1,
	mass_inventory = 0.1,
	model = "arrow-000",
	stacking = true,
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "fire arrow",
	count = 10,
	effect = "craftwood1",
	materials = {["log"] = 1},
	modes = {["wood"] = true}}

Itemspec{
	name = "fire arrow",
	categories = {["arrow"] = true},
	modifiers = {["physical damage"] = 5},
	icon = "arrow1",
	mass = 1,
	mass_inventory = 0.1,
	model = "arrow-000",
	special_effects = {"firearrow1"},
	stacking = true,
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "explosive arrow",
	count = 10,
	effect = "craftwood1",
	materials = {["log"] = 1},
	modes = {["wood"] = true}}

Itemspec{
	name = "explosive arrow",
	categories = {["arrow"] = true, ["explosive"] = true},
	modifiers = {["physical damage"] = 5},
	destroy_timer = 5,
	destroy_actions = {"explode"},
	icon = "arrow1",
	mass = 1,
	mass_inventory = 0.1,
	model = "arrow-000",
	stacking = true,
	water_gravity = Vector(0,6,0)}

AnimationProfileSpec{
	name = "default bow equipped",
	animations = {
		["attack back"] = "default attack bow",
		["attack front"] = "default attack bow",
		["attack left"] = "default attack bow",
		["attack right"] = "default attack bow",
		["attack stand"] = "default attack bow",
		["charge back"] = "default charge bow",
		["charge front"] = "default charge bow",
		["charge left"] = "default charge bow",
		["charge right"] = "default charge bow",
		["charge stand"] = "default charge bow",
		["hold"] = "default hold bow"}}

CraftingRecipeSpec{
	name = "wooden bow",
	effect = "craftwood1",
	materials = {["log"] = 3},
	modes = {["wood"] = true}}

Itemspec{
	name = "wooden bow",
	categories = {["ranged"] = true, ["weapon"] = true},
	actions = {["right"] = "ranged", ["left"] = "cancel ranged"},
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default bow equipped"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	icon = "bow1",
	modifiers = {["physical damage"] = 2},
	modifiers_bonus = {"ranged"},
	mass = 7,
	model = "bow1",
	timings = {["fire"] = 20},
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "fire bow",
	effect = "craftwood1",
	materials = {["log"] = 5},
	modes = {["wood"] = true}}

Itemspec{
	name = "fire bow",
	categories = {["ranged"] = true, ["weapon"] = true},
	actions = {["right"] = "ranged", ["left"] = "cancel ranged"},
	ammo_type = "fire arrow",
	animations_equipped = {["default"] = "default bow equipped"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	icon = "bow2",
	modifiers = {["physical damage"] = 4},
	modifiers_bonus = {"ranged"},
	mass = 7,
	model = "bow1", -- FIXME
	timings = {["fire"] = 20},
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "explosive bow",
	effect = "craftwood1",
	materials = {["log"] = 5},
	modes = {["wood"] = true}}

Itemspec{
	name = "explosive bow",
	categories = {["ranged"] = true, ["weapon"] = true},
	actions = {["right"] = "ranged", ["left"] = "cancel ranged"},
	ammo_type = "explosive arrow",
	animations_equipped = {["default"] = "default bow equipped"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	icon = "bow2",
	modifiers = {["physical damage"] = 4},
	modifiers_bonus = {"ranged"},
	mass = 7,
	model = "bow1", -- FIXME
	timings = {["fire"] = 20},
	water_gravity = Vector(0,6,0)}
