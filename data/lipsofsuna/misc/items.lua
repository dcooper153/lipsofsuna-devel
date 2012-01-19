------------------------------------------------------------------------------
-- Materials.

-- * materials (label used below in "weapons" section)
-- tile \ state 	label	stone	ore	ingot
-- ==================================================
-- adamantium 		A	4	-	-
-- aquanite 		Q	2	-	-
-- crimson		C	4	-	-
-- granite/ferro/iron	S	5	4	3
-- basalt		S	5	-	-
-- sand chunk/../glass	-	3	-	2
-- soil chunk		-	3	-	-

-- non generating admin items: "weapons", "logs", "health jar", "magic jar"

Itemspec{
	name = "berry",
	model = "fruit1",
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "flax",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "fruit",
	model = "fruit1",
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "glass ingot",
	model = "book-000", -- FIXME
	icon = "ingot1",
	categories = {"material"},
	crafting_materials = {["sand chunk"] = 1},
	health = 10,
	mass = 1.5,
	stacking = true}

Itemspec{
	name = "iron ingot",
	model = "book-000", -- FIXME
	icon = "ingot1",
	mass = 2,
	categories = {"build", "material"},
	construct_tile = "iron1",
	construct_tile_count = 1,
	crafting_materials = {["ferrostone"] = 1},
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "hide",
	model = "book-000", -- FIXME
	icon = "hide1",
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "milky powder",
	model = "book-000", -- FIXME
	icon = "potion3",
	categories = {"material"},
	crafting_count = 20,
	description = "A magical reagent",
	crafting_materials = {["mushroom"] = 1},
	mass = 1,
	mass_inventory = 0.05,
	stacking = true}

Itemspec{
	name = "wheat",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

------------------------------------------------------------------------------
-- Ammo.

Itemspec{
	name = "bullet",
	model = "arrow-000", -- FIXME
	categories = {"bullet"},
	influences_base = {physical = -1},
	crafting_count = 10,
	crafting_materials = {["iron ingot"] = 1, ["crimson stone"] = 1},
	gravity_projectile = Vector(0,-4,0),
	icon = "arrow1",
	mass = 1,
	mass_inventory = 0.1,
	stacking = true}

------------------------------------------------------------------------------
-- Decorations

Itemspec{
	name = "broken wooden fence",
	model = "fence1",
	icon = "chest1", -- FIXME
	mass = 10,
	categories = {"furniture"},
	crafting_materials = {log = 7},
	effect_craft = "craftwood1",
	health = 50,
	stacking = false,
	water_gravity = Vector(0,6,0)}

------------------------------------------------------------------------------
-- FIXME

Itemspec{
	name = "gem",
	icon = "gem1",
	model = "gem-000",
	mass = 2,
	categories = {"reagent"},
	stacking = true}
