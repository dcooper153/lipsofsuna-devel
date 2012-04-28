Program:add_path(Mod.path)

Itemspec{
	name = "boomerang",
	animations = {["fly start"] = "boomerang fly start", ["fly stop"] = "boomerang fly stop"},
	mass = 2,
	model = "boomerang1",
	icon = "boomerang1",
	categories = {["boomerang"] = true, ["throwable"] = true, ["weapon"] = true},
	crafting_materials = {["log"] = 2},
	effect_craft = "craftwood1",
	gravity_projectile = Vector(0,2,0),
	influences = {["physical damage"] = 5},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	stacking = true,
	water_gravity = Vector(0,6,0)}
