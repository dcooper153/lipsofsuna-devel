Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "dress",
	effect = "craftsewing1",
	materials = {["hide"] = 3}}

Itemspec{
	name = "dress",
	model = "chinadress1",
	icon = "cloth1",
	armor_class = 0.02,
	categories = {["armor"] = true},
	equipment_models = {
		["aer"] = {upper_safe = "", upper1 = "aerchinadress1", panties = "panties2"},
		["kraken"] = {upper1 = "aerchinadress1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}
