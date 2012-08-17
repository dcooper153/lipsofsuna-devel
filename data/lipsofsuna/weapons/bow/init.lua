Program:add_path(Mod.path)

Effectspec{
	name = "firearrow1",
	node = "#root",
	light = true,
	light_diffuse = {1,1,1,1},
	light_equation = {1,0.1,0.01}}

Itemspec{
	name = "arrow",
	model = "arrow-000",
	categories = {["arrow"] = true},
	influences = {["physical damage"] = 5},
	crafting_count = 10,
	crafting_materials = {["log"] = 1},
	gravity_projectile = Vector(0,-8,0),
	icon = "arrow1",
	mass = 1,
	mass_inventory = 0.1,
	stacking = true,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "fire arrow",
	model = "arrow-000",
	categories = {["arrow"] = true},
	influences = {["physical damage"] = 5},
	crafting_count = 10,
	crafting_materials = {["log"] = 1},
	icon = "arrow1",
	mass = 1,
	mass_inventory = 0.1,
	stacking = true,
	special_effects = {"firearrow1"},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "explosive arrow",
	model = "arrow-000",
	categories = {["arrow"] = true, ["explosive"] = true},
	influences = {["physical damage"] = 5},
	crafting_count = 10,
	crafting_materials = {["log"] = 1},
	destroy_timer = 5,
	destroy_actions = {"explode"},
	icon = "arrow1",
	mass = 1,
	mass_inventory = 0.1,
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

Itemspec{
	name = "wooden bow",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "arrow",
	animations_equipped = {["default"] = "default bow equipped"},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	icon = "bow1",
	influences = {["physical damage"] = 4},
	influences_bonus = {"ranged"},
	mass = 7,
	model = "bow1",
	timings = {["fire"] = 20},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "fire bow",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "fire arrow",
	animations_equipped = {["default"] = "default bow equipped"},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	icon = "bow2",
	influences = {["physical damage"] = 4},
	influences_bonus = {"ranged"},
	mass = 7,
	model = "bow1", -- FIXME
	timings = {["fire"] = 20},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "explosive bow",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "explosive arrow",
	animations_equipped = {["default"] = "default bow equipped"},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	icon = "bow2",
	influences = {["physical damage"] = 4},
	influences_bonus = {"ranged"},
	mass = 7,
	model = "bow1", -- FIXME
	timings = {["fire"] = 20},
	water_gravity = Vector(0,6,0)}
