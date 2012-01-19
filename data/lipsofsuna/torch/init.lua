Program:add_path(Mod.path)

Effect{
	name = "torch1",
	sound = "torchlit1"}

Itemspec{
	name = "torch",
	model = "torch1",
	animation_hold = "hold torch",
	categories = {"misc"},
	crafting_materials = {["log"] = 1},
	effect_equip = "torch1",
	effect_unequip = "torch1",
	equipment_slot = "hand.L",
	icon = "torch1",
	mass = 3,
	special_effects = {
		{node = "#flame", model = "torchfx1"},
		{node = "#flame", type = "light", equation = {2,0.1,0.1}, diffuse = {1,1,1,1}}},
	water_gravity = Vector(0,6,0)}
