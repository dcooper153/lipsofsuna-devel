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

Itemspec{
	name = "wooden bow",
	model = "bow1",
	icon = "bow1",
	ammo_type = "arrow",
	animation_attack = "attack bow",
	animation_charge = "charge bow",
	animation_hold = "hold bow",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	influences = {["physical damage"] = 4},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	water_gravity = Vector(0,6,0),
	mass = 7}

Itemspec{
	name = "fire bow",
	model = "bow1", -- FIXME
	icon = "bow2",
	ammo_type = "fire arrow",
	animation_attack = "attack bow",
	animation_charge = "charge bow",
	animation_hold = "hold bow",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	influences = {["physical damage"] = 4},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	water_gravity = Vector(0,6,0),
	mass = 7}

Itemspec{
	name = "explosive bow",
	model = "bow1", -- FIXME
	icon = "bow2",
	ammo_type = "explosive arrow",
	animation_attack = "attack bow",
	animation_charge = "charge bow",
	animation_hold = "hold bow",
	categories = {["ranged"] = true, ["weapon"] = true},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	influences = {["physical damage"] = 4},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	water_gravity = Vector(0,6,0),
	mass = 7}
