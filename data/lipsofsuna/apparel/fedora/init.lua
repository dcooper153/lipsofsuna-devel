Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "fedora",
	effect = "craftsewing1",
	materials = {["hide"] = 2}}

Itemspec{
	name = "fedora",
	model = "fedora1",
	icon = "leatherarmor1",
	armor_class = 0.02,
	categories = {["armor"] = true},
	equipment_models = {
		["aer"] = {hat = "aerfedora1"},
		["kraken"] = {hat = "aerfedora1"}},
	equipment_slot = "head",
	mass = 5,
	water_gravity = Vector(0,6,0)}
