Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "leather leggings",
	effect = "craftsewing1",
	materials = {["hide"] = 4}}

Itemspec{
	name = "leather leggings",
	model = "leggings1",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	equipment_models = {
		["aer"] = {feet = "", legs = "aerleggings1"}},
	equipment_slot = "feet",
	mass = 5,
	water_gravity = Vector(0,6,0)}
