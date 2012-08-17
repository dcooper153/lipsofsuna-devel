Program:add_path(Mod.path)

Questspec{
	name = "Down to the Dungeons"}

Questspec{
	name = "An Introduction to Crafting"}

Questspec{
	name = "An Introduction to Magic"}

Questspec{
	name = "An Introduction to Ultraviolence"}

Itemspec{
	name = "dying torch",
	model = "torch1",
	animations_equipped = {["default"] = "default torch equipped"},
	categories = {["quest"] = true},
	crafting_materials = {["log"] = 1},
	effect_equip = "torch1",
	effect_unequip = "torch1",
	equipment_slot = "hand.L",
	icon = "torch1",
	important = true,
	mass = 3,
	special_effects = {"torchflame1"},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "combat token",
	icon = "gem1",
	model = "gem-000",
	mass = 2,
	categories = {["quest"] = true},
	important = true,
	stacking = true}

Actorspec{
	name = "Dummy",
	base = "devora",
	ai_enable_combat = false,
	ai_enable_wander = false,
	marker = "dummy",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["combat token"] = 1},
	preset = "Female 3"}

require(Mod.path .. "fa")
require(Mod.path .. "spud")
require(Mod.path .. "lyra")
require(Mod.path .. "xsphin")
require(Mod.path .. "zyra")
require(Mod.path .. "giantshaft")
