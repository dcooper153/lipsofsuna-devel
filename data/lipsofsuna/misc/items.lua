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

local CraftingRecipeSpec = require("core/specs/crafting-recipe")
local ItemSpec = require("core/specs/item")

------------------------------------------------------------------------------
-- Decorations

CraftingRecipeSpec{
	name = "broken wooden fence",
	effect = "craftwood1",
	materials = {["log"] = 7},
	modes = {["wood"] = true}}

ItemSpec{
	name = "broken wooden fence",
	model = "fence1",
	icon = "chest1", -- FIXME
	mass = 10,
	categories = {["furniture"] = true},
	health = 50,
	stacking = false,
	water_gravity = Vector(0,6,0)}
