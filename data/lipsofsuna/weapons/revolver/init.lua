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

CraftingRecipeSpec{
	name = "adamantium revolver",
	effect = "craftmetal1",
	materials = {["adamantium"] = 10, ["log"] = 1},
	modes = {["metal"] = true}}

Itemspec{
	name = "adamantium revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	actions = {["right"] = "ranged", ["left"] = "cancel ranged"},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default revolver equipped"},
	effect_attack = "revolver1",
	icon = "revolver1",
	modifiers = {["physical damage"] = 18},
	modifiers_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8,
	model = "revolver1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "aquanite revolver",
	effect = "craftmetal1",
	materials = {["aquanite stone"] = 10, ["log"] = 1},
	modes = {["metal"] = true}}

Itemspec{
	name = "aquanite revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	actions = {["right"] = "ranged", ["left"] = "cancel ranged"},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default revolver equipped"},
	effect_attack = "revolver1",
	icon = "revolver1",
	modifiers = {["cold damage"] = 7, ["physical damage"] = 7},
	modifiers_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8,
	model = "revolver1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "crimson revolver",
	effect = "craftmetal1",
	materials = {["crimson stone"] = 10, ["log"] = 1},
	modes = {["metal"] = true}}

Itemspec{
	name = "crimson revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	actions = {["right"] = "ranged", ["left"] = "cancel ranged"},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default revolver equipped"},
	effect_attack = "revolver1",
	icon = "revolver1",
	modifiers = {["fire damage"] = 7, ["physical damage"] = 7},
	modifiers_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8,
	model = "revolver1",
	timings = {["fire"] = 20}}

CraftingRecipeSpec{
	name = "iron revolver",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 4, ["log"] = 1},
	modes = {["metal"] = true}}

Itemspec{
	name = "iron revolver",
	categories = {["ranged"] = true, ["weapon"] = true},
	actions = {["right"] = "ranged", ["left"] = "cancel ranged"},
	ammo_type = "bullet",
	animations_equipped = {["default"] = "default revolver equipped"},
	effect_attack = "revolver1",
	icon = "revolver1",
	modifiers = {["physical damage"] = 10},
	modifiers_bonus = {"ranged", "guns"},
	equipment_slot = "hand.R",
	mass = 8,
	model = "revolver1",
	timings = {["fire"] = 20}}
