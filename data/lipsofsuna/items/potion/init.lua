Program:add_path(Mod.path)

Itemspec{
	name = "empty bottle",
	model = "emptypotion1",
	categories = {["misc"] = true},
	crafting_materials = {["glass ingot"] = 1},
	health = 5,
	mass = 1,
	icon = "potion3", -- FIXME
	stacking = true}

Itemspec{
	name = "cure disease potion",
	model = "potion1",
	mass = 2,
	action_use = "curedisease",
	categories = {["potion"] = true},
	crafting_materials = {["empty bottle"] = 1, ["dewspring leaf"] = 1},
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	stacking = true,
	icon = "potion2",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "health potion",
	model = "potion3",
	mass = 2,
	action_use = "recoverhealth",
	categories = {["potion"] = true},
	crafting_materials = {["empty bottle"] = 1, ["milky powder"] = 1},
	description = "Right click to heal yourself.",
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	stacking = true,
	icon = "potion1",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "mana potion",
	model = "potion7",
	mass = 2,
	action_use = "recovermana",
	categories = {["potion"] = true},
	crafting_materials = {["empty bottle"] = 1, ["dewspring leaf"] = 1},
	description = "Right click to replenish your willpower.",
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	stacking = true,
	icon = "potion2",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "poison potion",
	model = "potion2",
	mass = 2,
	categories = {["potion"] = true},
	crafting_materials = {["empty bottle"] = 1, ["dewspring leaf"] = 1},
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	stacking = true,
	icon = "potion2",
	water_gravity = Vector(0,6,0)}
