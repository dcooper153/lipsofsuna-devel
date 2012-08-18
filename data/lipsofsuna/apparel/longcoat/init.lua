Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "long coat",
	effect = "craftsewing1",
	materials = {["hide"] = 3}}

Itemspec{
	name = "long coat",
	categories = {["armor"] = true},
	armor_class = 0.05,
	equipment_models = {
		["aer"] = {["arms"] = "", ["upper1"] = "longcoat1", ["upper_safe"] = ""}},
	equipment_slot = "upperbody",
	icon = "lizardboot1",
	mass = 4,
	model = "longcoat1",
	water_gravity = Vector(0,6,0)}
