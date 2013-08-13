Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "leather helm",
	effect = "craftsewing1",
	materials = {["hide"] = 2}}

Itemspec{
	name = "leather helm",
	model = "book-000",
	icon = "leatherhelm1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	equipment_models = {["aermale"] = {hair = "aermaleleatherhead2"}},
	equipment_slot = "head",
	mass = 3,
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "leather gloves",
	effect = "craftsewing1",
	materials = {["hide"] = 2}}

Itemspec{
	name = "leather gloves",
	model = "book-000",
	icon = "leatherglove1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	equipment_models = {["aermale"] = {hands = "aermaleleatherarms2"}},
	equipment_slot = "arms",
	mass = 3,
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "leather boots",
	effect = "craftsewing1",
	materials = {["hide"] = 4}}

Itemspec{
	name = "leather boots",
	model = "boots1",
	icon = "leatherboot1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	equipment_models = {
		-- TODO: Needs to be redone to fit the new player model.
		--["aer"] = {feet = "boots1"},
		["aermale"] = {feet = "aermaleboots1"}},
	equipment_slot = "feet",
	mass = 3,
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "leather pants",
	effect = "craftsewing1",
	materials = {["hide"] = 4}}

Itemspec{
	name = "leather pants",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	equipment_models = {
		["aermale"] = {legs = "aermaleleatherlegs2"}},
	equipment_textures =
	{
		["aer"] = {["aer1"] = "aerspats1"}
	},
	equipment_slot = "pants",
	mass = 5,
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "leather top",
	effect = "craftsewing1",
	materials = {["hide"] = 4}}

Itemspec{
	name = "leather top",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "leather skirt",
	effect = "craftsewing1",
	materials = {["hide"] = 4}}

Itemspec{
	name = "leather skirt",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	equipment_models = {
		["aer"] = {skirt = "aerleatherskirt1", panties = "panties2"},
		["kraken"] = {skirt = "aerleatherskirt1"}},
	equipment_slot = "lowerbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

CraftingRecipeSpec{
	name = "leather chest",
	effect = "craftsewing1",
	materials = {["hide"] = 4}}

Itemspec{
	name = "leather chest",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {["armor"] = true},
	equipment_models = {
		-- TODO: Needs to be redone to fit the new player model.
		--["aer"] = {upper1 = "aerleatherchest1"},
		--["kraken"] = {upper1 = "aerleatherchest1"},
		["aermale"] = {upper = "aermaleleatherchest2"}},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}
