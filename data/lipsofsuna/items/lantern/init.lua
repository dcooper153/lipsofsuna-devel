Program:add_path(Mod.path)

Effectspec{
	name = "candleflame1",
	light = true,
	light_diffuse = {1,1,1,1},
	light_equation = {2,0.1,0.1},
	node = "#flame",
	particle = "candlefx1"}

CraftingRecipeSpec{
	name = "wooden lantern",
	effect = "craftwood1",
	materials = {["log"] = 2},
	modes = {["wood"] = true}}

Itemspec{
	name = "wooden lantern",
	animations_equipped = {["default"] = "default torch equipped"},
	description = "A wooden lantern commonly used as a ligth source",
	equipment_anchor = "#handle",
	equipment_slot = "hand.L",
	icon = "potion3",
	mass = 3,
	mass_inventory = 0.05,
	model = "lantern1",
	special_effects = {"candleflame1"},
	stacking = true}
