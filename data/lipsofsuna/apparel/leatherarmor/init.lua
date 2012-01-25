Itemspec{
	name = "leather helm",
	model = "book-000",
	icon = "leatherhelm1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 2},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aermale"] = {hair = "aermale1leatherhead2"}},
	equipment_slot = "head",
	mass = 3,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "leather gloves",
	model = "book-000",
	icon = "leatherglove1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {["hide"] = 2},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aermale"] = {hands = "aermale1leatherarms2"}},
	equipment_slot = "arms",
	mass = 3,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "leather boots",
	model = "boots1",
	icon = "leatherboot1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		-- TODO: Needs to be redone to fit the new player model.
		--["aer/android/devora/wyrm"] = {feet = "boots1"},
		["aermale"] = {feet = "aermale1boots1"}},
	equipment_slot = "feet",
	mass = 3,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "leather pants",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/devora/wyrm"] = {skirt = "aerskirt1", panties = "panties2"},
		["kraken"] = {skirt = "aerskirt1"},
		["aermale"] = {legs = "aermale1leatherlegs2"}},
	equipment_slot = "lowerbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "leather top",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/devora/kraken/wyrm"] = {upper_safe = false, upper1 = "aerbra1"}},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Program:add_path(Mod.path)

Itemspec{
	name = "leather skirt",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/devora/wyrm"] = {skirt = "aerleatherskirt1", panties = "panties2"},
		["kraken"] = {skirt = "aerleatherskirt1"}},
	equipment_slot = "lowerbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "leather chest",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		-- TODO: Needs to be redone to fit the new player model.
		--["aer/android/devora/kraken/wyrm"] = {upper1 = "aerleatherchest1"},
		["aermale"] = {upper = "aermale1leatherchest2"}},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}
