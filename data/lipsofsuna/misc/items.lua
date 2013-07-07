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

------------------------------------------------------------------------------
-- Ammo.

CraftingRecipeSpec{
	name = "bullet",
	effect = "craftmetal1",
	count = 10,
	materials = {["iron ingot"] = 1},
	modes = {["metal"] = true}}

Itemspec{
	name = "bullet",
	categories = {["bullet"] = true},
	gravity_projectile = Vector(0,-4,0),
	icon = "arrow1",
	modifiers = {["physical damage"] = 1},
	mass = 1,
	mass_inventory = 0.1,
	model = "arrow-000", -- FIXME
	stacking = true}

------------------------------------------------------------------------------
-- Decorations

CraftingRecipeSpec{
	name = "broken wooden fence",
	effect = "craftwood1",
	materials = {["log"] = 7},
	modes = {["wood"] = true}}

Itemspec{
	name = "broken wooden fence",
	model = "fence1",
	icon = "chest1", -- FIXME
	mass = 10,
	categories = {["furniture"] = true},
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
	categories = {["reagent"] = true},
	stacking = true}
