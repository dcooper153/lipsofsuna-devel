Program:add_path(Mod.path)

Itemspec{
	name = "boomerang",
	animations = {
		["fly start"] = {animation = "fly", channel = 1, permanent = true},
		["fly stop"] = {channel = 1, permanent = true}},
	mass = 2,
	model = "boomerang1",
	icon = "boomerang1",
	categories = {"boomerang", "throwable", "weapon"},
	crafting_materials = {log = 2},
	effect_craft = "craftwood1",
	gravity_projectile = Vector(0,2,0),
	influences_base = {physical = -5},
	influences_bonus = {"ranged"},
	equipment_slot = "hand.R",
	stacking = true,
	water_gravity = Vector(0,6,0)}
