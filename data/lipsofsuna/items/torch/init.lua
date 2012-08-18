Program:add_path(Mod.path)

Effectspec{
	name = "torch1",
	sound = "torchlit1"}

Effectspec{
	name = "torchflame1",
	light = true,
	light_diffuse = {1,1,1,1},
	light_equation = {2,0.1,0.1},
	node = "#flame",
	particle = "torchfx1"}

AnimationProfileSpec{
	name = "default torch equipped",
	animations = {
		["hold"] = "default hold torch"}}

CraftingRecipeSpec{
	name = "torch",
	effect = "craftwood1",
	materials = {["log"] = 1},
	modes = {["wood"] = true, ["default"] = true}}

Itemspec{
	name = "torch",
	categories = {["misc"] = true},
	animations_equipped = {["default"] = "default torch equipped"},
	effect_equip = "torch1",
	effect_unequip = "torch1",
	equipment_slot = "hand.L",
	icon = "torch1",
	mass = 3,
	model = "torch1",
	special_effects = {"torchflame1"},
	water_gravity = Vector(0,6,0)}
