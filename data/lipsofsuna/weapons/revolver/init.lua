Program:add_path(Mod.path)

Effectspec{
	name = "revolver1",
	sound = "weaponmusket1"} -- TODO: Separate sound?

AnimationProfileSpec{
	name = "default revolver equipped",
	animations = {
		["attack back"] = "default attack revolver",
		["attack front"] = "default attack revolver",
		["attack left"] = "default attack revolver",
		["attack right"] = "default attack revolver",
		["attack stand"] = "default attack revolver",
		["charge back"] = "default charge revolver",
		["charge front"] = "default charge revolver",
		["charge left"] = "default charge revolver",
		["charge right"] = "default charge revolver",
		["charge stand"] = "default charge revolver",
		["hold"] = "default hold revolver"}}

Itemspec{
	name = "adamantium revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default revolver equipped"},
	crafting_materials = {["adamantium"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	icon = "revolver1",
	influences = {["physical damage"] = 18},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8,
	model = "revolver1",
	timings = {["fire"] = 20}}

Itemspec{
	name = "aquanite revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default revolver equipped"},
	crafting_materials = {["aquanite stone"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	icon = "revolver1",
	influences = {["cold damage"] = 7, ["physical damage"] = 7},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8,
	model = "revolver1",
	timings = {["fire"] = 20}}

Itemspec{
	name = "crimson revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default revolver equipped"},
	crafting_materials = {["crimson stone"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	icon = "revolver1",
	influences = {["fire damage"] = 7, ["physical damage"] = 7},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8,
	model = "revolver1",
	timings = {["fire"] = 20}}

Itemspec{
	name = "iron revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default revolver equipped"},
	crafting_materials = {["iron ingot"] = 10, ["log"] = 1},
	effect_attack = "revolver1",
	icon = "revolver1",
	influences = {["physical damage"] = 10},
	influences_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8,
	model = "revolver1",
	timings = {["fire"] = 20}}
