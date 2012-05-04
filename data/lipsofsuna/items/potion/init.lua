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
	categories = {["potion"] = true},
	crafting_materials = {["empty bottle"] = 1, ["dewspring leaf"] = 1},
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	icon = "potion2",
	potion_effects = {["cure disease"] = 1},
	stacking = true,
	usages = {["drink"] = true},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "health potion",
	model = "potion3",
	mass = 2,
	categories = {["potion"] = true},
	crafting_materials = {["empty bottle"] = 1, ["milky powder"] = 1},
	description = "Right click to heal yourself.",
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	icon = "potion1",
	potion_effects = {["restore health"] = 20},
	stacking = true,
	usages = {["drink"] = true},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "mana potion",
	model = "potion7",
	mass = 2,
	categories = {["potion"] = true},
	crafting_materials = {["empty bottle"] = 1, ["dewspring leaf"] = 1},
	description = "Right click to replenish your willpower.",
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	icon = "potion2",
	potion_effects = {["restore willpower"] = 20},
	stacking = true,
	usages = {["drink"] = true},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "poison potion",
	model = "potion2",
	mass = 2,
	categories = {["potion"] = true},
	crafting_materials = {["empty bottle"] = 1, ["dewspring leaf"] = 1},
	effect_craft = "craftpotion1",
	effect_use = "potiondrink1",
	icon = "potion2",
	potion_effects = {["poison"] = 10},
	stacking = true,
	usages = {["drink"] = true},
	water_gravity = Vector(0,6,0)}
