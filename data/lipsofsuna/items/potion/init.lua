Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "empty bottle",
	effect = "craftmetal1",
	materials = {["glass ingot"] = 1},
	modes = {["alchemy"] = true, ["metal"] = true}}

Itemspec{
	name = "empty bottle",
	model = "emptypotion1",
	categories = {["misc"] = true},
	health = 5,
	mass = 1,
	icon = "potion3", -- FIXME
	stacking = true}

CraftingRecipeSpec{
	name = "cure disease potion",
	effect = "craftalchemy1",
	materials = {["empty bottle"] = 1, ["dewspring leaf"] = 1},
	modes = {["alchemy"] = true}}

Itemspec{
	name = "cure disease potion",
	model = "potion1",
	mass = 2,
	categories = {["potion"] = true},
	effect_use = "potiondrink1",
	icon = "potion2",
	potion_effects = {["cure disease"] = 1},
	stacking = true,
	usages = {["drink"] = true},
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "health potion",
	effect = "craftalchemy1",
	materials = {["empty bottle"] = 1, ["watermelon"] = 1},
	modes = {["alchemy"] = true}}

Itemspec{
	name = "health potion",
	model = "potion3",
	mass = 2,
	categories = {["potion"] = true},
	description = "Drink to heal yourself.",
	effect_use = "potiondrink1",
	icon = "potion1",
	potion_effects = {["restore health"] = 20},
	stacking = true,
	usages = {["drink"] = true},
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "mana potion",
	effect = "craftalchemy1",
	materials = {["empty bottle"] = 1, ["dewspring leaf"] = 1},
	modes = {["alchemy"] = true}}

Itemspec{
	name = "mana potion",
	model = "potion7",
	mass = 2,
	categories = {["potion"] = true},
	description = "Drink to replenish your willpower.",
	effect_use = "potiondrink1",
	icon = "potion2",
	potion_effects = {["restore willpower"] = 20},
	stacking = true,
	usages = {["drink"] = true},
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "poison potion",
	effect = "craftalchemy1",
	materials = {["empty bottle"] = 1, ["dewspring leaf"] = 1},
	modes = {["alchemy"] = true}}

Itemspec{
	name = "poison potion",
	model = "potion2",
	mass = 2,
	categories = {["potion"] = true},
	effect_use = "potiondrink1",
	icon = "potion2",
	potion_effects = {["poison"] = 10},
	stacking = true,
	usages = {["drink"] = true},
	water_gravity = Vector(0,6,0)}
