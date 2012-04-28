Program:add_path(Mod.path)

Itemspec{
	name = "air tank",
	model = "airtank1",
	mass = 15,
	categories = {["explosive"] = true},
	crafting_materials = {["log"] = 1},
	destroy_actions = {"explode"},
	effect_craft = "craftmetal1",
	health = 1,
	stacking = false,
	water_gravity = Vector(0,6,0)}
