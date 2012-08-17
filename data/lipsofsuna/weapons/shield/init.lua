Program:add_path(Mod.path)

AnimationProfileSpec{
	name = "default shield equipped",
	animations = {
		["hold"] = "default hold shield"}}

Itemspec{
	name = "round shield",
	animations_equipped = {["default"] = "default shield equipped"},
	armor_class = 0.08,
	categories = {["shield"] = true},
	crafting_materials = {["log"] = 3, ["iron ingot"] = 3},
	equipment_slot = "hand.L",
	icon = "woodshield1",
	mass = 24,
	model = "shield1"}

Itemspec{
	name = "wooden shield",
	animations_equipped = {["default"] = "default shield equipped"},
	armor_class = 0.08,
	categories = {["shield"] = true},
	crafting_materials = {["log"] = 1},
	equipment_slot = "hand.L",
	icon = "woodshield2",
	mass = 7,
	model = "shield-001-wood1",
	water_gravity = Vector(0,6,0)}
