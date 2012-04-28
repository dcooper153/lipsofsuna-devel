Program:add_path(Mod.path)

Itemspec{
	name = "workbench",
	model = "workbench1",
	icon = "workbench1",
	mass = 21,
	action_use = "loot",
	animation_looted = "open",
	animation_looting = "opening",
	categories = {["container"] = true, ["workbench"] = true},
	crafting_materials = {["log"] = 7},
	effect_craft = "craftwood1",
	inventory_size = 4,
	inventory_type = "workbench",
	stacking = false,
	water_gravity = Vector(0,6,0)}
