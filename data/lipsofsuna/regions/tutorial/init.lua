Program:add_path(Mod.path)

Quest{name = "Down to the Dungeons"}
Quest{name = "An Introduction to Crafting"}
Quest{name = "An Introduction to Magic"}
Quest{name = "An Introduction to Ultraviolence"}

Itemspec{
	name = "dying torch",
	model = "torch1",
	animation_hold = "hold torch",
	categories = {"quest"},
	crafting_materials = {["log"] = 1},
	effect_equip = "torch1",
	effect_unequip = "torch1",
	equipment_slot = "hand.L",
	icon = "torch1",
	mass = 3,
	special_effects = {
		{node = "#flame", model = "torchfx1"},
		{node = "#flame", type = "light", equation = {2,0.2,0.2}, diffuse = {1,1,1,1}}},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "combat token",
	icon = "gem1",
	model = "gem-000",
	mass = 2,
	categories = {"quest"},
	stacking = true}

Species{
	name = "Dummy",
	base = "devora",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "dummy",
	marker = "dummy",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"combat token"}}

require(Mod.path .. "fa")
require(Mod.path .. "spud")
require(Mod.path .. "lyra")
require(Mod.path .. "xsphin")
require(Mod.path .. "zyra")
require(Mod.path .. "giantshaft")
