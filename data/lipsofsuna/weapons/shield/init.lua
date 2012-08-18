Program:add_path(Mod.path)

AnimationProfileSpec{
	name = "default shield equipped",
	animations = {
		["hold"] = "default hold shield"}}

CraftingRecipeSpec{
	name = "round shield",
	effect = "craftmetal1",
	materials = {["log"] = 3, ["iron ingot"] = 3},
	modes = {["metal"] = true, ["wood"] = true}}

Itemspec{
	name = "round shield",
	animations_equipped = {["default"] = "default shield equipped"},
	armor_class = 0.08,
	categories = {["shield"] = true},
	equipment_slot = "hand.L",
	icon = "woodshield1",
	mass = 24,
	model = "shield1"}

CraftingRecipeSpec{
	name = "wooden shield",
	effect = "craftwood1",
	materials = {["log"] = 2},
	modes = {["wood"] = true}}

Itemspec{
	name = "wooden shield",
	animations_equipped = {["default"] = "default shield equipped"},
	armor_class = 0.08,
	categories = {["shield"] = true},
	equipment_slot = "hand.L",
	icon = "woodshield2",
	mass = 7,
	model = "shield-001-wood1",
	water_gravity = Vector(0,6,0)}
