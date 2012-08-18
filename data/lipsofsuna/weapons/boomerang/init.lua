Program:add_path(Mod.path)

Animationspec{
	name = "boomerang fly start",
	animation = "boomerang1-fly",
	channel = 1,
	permanent = true}

Animationspec{
	name = "boomerang fly stop",
	channel = 1,
	permanent = true}

AnimationProfileSpec{
	name = "boomerang",
	animations = {
		["fly start"] = "boomerang fly start",
		["fly stop"] = "boomerang fly stop"}}

CraftingRecipeSpec{
	name = "boomerang",
	effect = "craftwood1",
	materials = {["log"] = 2},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "boomerang",
	categories = {["boomerang"] = true, ["throwable"] = true, ["weapon"] = true},
	animations = {["default"] = "boomerang"},
	mass = 2,
	model = "boomerang1",
	icon = "boomerang1",
	gravity_projectile = Vector(0,2,0),
	influences = {["physical damage"] = 5},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	stacking = true,
	water_gravity = Vector(0,6,0)}
