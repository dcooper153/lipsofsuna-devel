Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "kitty headband",
	effect = "craftsewing1",
	materials = {["hide"] = 2}}

Itemspec{
	name = "kitty headband",
	model = "headband1",
	armor_class = 0.02,
	categories = {["armor"] = true},
	equipment_models = {
		["aer"] = {headband = "aerheadband1"},
		["kraken"] = {headband = "aerheadband1"}},
	equipment_slot = "head",
	mass = 1,
	water_gravity = Vector(0,6,0)}
