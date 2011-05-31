Itemspec{
	name = "dress",
	model = "chinadress1",
	icon = "cloth1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {log = 1},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "aerchinadress1", panties = "panties2"},
		["android"] = {upper1 = "androidchinadress1"},
		["devora"] = {upper1 = "devorachinadress1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "fedora",
	model = "fedora1",
	icon = "leatherarmor1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {hide = 2},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/kraken/wyrm"] = {hat = "aerfedora1"},
		["devora"] = {hat = "devorafedora1"}},
	equipment_slot = "head",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "trog rags",
	model = "book-000",
	icon = "cloth1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {log = 1},
	effect_craft = "craftsewing1",
	equipment_models = {
		["troglodyte female"] = {upper1 = "trogfemrags1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "iron greaves",
	model = "legplate1",
	icon = "metalboot1",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 3},
	effect_craft = "craftmetal1",
	equipment_models = {
		["aer/kraken/wyrm"] = {feet = false, legs = "aerlegplate1"},
		["android"] = {feet = false, legs = "androidlegplate1"}},
	equipment_slot = "feet",
	mass = 15}

Itemspec{
	name = "iron gauntlets",
	model = "armplate1",
	icon = "metalglove1",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 2},
	effect_craft = "craftmetal1",
	equipment_models = {
		["aer/kraken/wyrm"] = {arms = "aerarmplate1", hands = false},
		["android"] = {arms = "androidarmplate1", hands = false}},
	equipment_slot = "arms",
	mass = 10}

Itemspec{
	name = "iron breastplate",
	model = "upperplate1",
	icon = "metalarmor1",
	armor_class = 0.18,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 5},
	effect_craft = "craftmetal1",
	equipment_models = {
		["aer/kraken/wyrm"] = {upper = "aerupperplate1", lower = "aerlowerplate1"},
		["android"] = {upper = "androidupperplate1", lower = "androidlowerplate1"}},
	equipment_slot = "upperbody",
	equipment_slots_reserved = {"lowerbody"},
	mass = 25}

Itemspec{
	name = "kitty headband",
	model = "headband1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {hide = 2},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/kraken/wyrm"] = {headband = "aerheadband1"},
		["devora"] = {headband = "devoraheadband1"}},
	equipment_slot = "head",
	mass = 1,
	water_gravity = Vector(0,6,0)}

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
	name = "leather leggings",
	model = "leggings1",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/kraken/wyrm"] = {feet = false, legs = "aerleggings1"},
		["android"] = {feet = false, legs = "androidleggings1"},
		["devora"] = {feet = false, legs = "devoraleggings1"}},
	equipment_slot = "feet",
	mass = 5,
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
		["aer/android/kraken/wyrm"] = {legs = "aerlegarmor2", legs1 = "boots1"},["aermale"] = {feet = "aermale1boots1"}},
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
		["aer/kraken/wyrm"] = {skirt = "aerlowerarmor1", panties = "panties2"},["aermale"] = {legs = "aermale1leatherlegs2"}},
	equipment_slot = "lowerbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "leather brigand pants",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/kraken/wyrm"] = {skirt = "aerlowerarmor1", panties = "panties2"},["aermale"] = {legs = "aermale1leatherlegs1"}},
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
		["aer/kraken/wyrm"] = {upper1 = "bra1"}},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "leather skirt",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/kraken/wyrm"] = {skirt = "aerleatherskirt1", panties = "panties2"}},
	equipment_slot = "lowerbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "lizard boots",
	armor_class = 0.05,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/kraken/wyrm"] = {feet = "aerlizardboots1"}},
	equipment_slot = "feet",
	icon = "lizardboot1",
	mass = 4,
	model = "lizardboots1",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "lizard dress",
	armor_class = 0.1,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 6},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/kraken/wyrm"] = {dress1 = "aerlizarddress1", panties = "panties2"}},
	equipment_slot = "upperbody",
	icon = "lizardarmor1",
	mass = 7,
	model = "lizarddress1",
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "lizard gloves",
	armor_class = 0.05,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/android/kraken/wyrm"] = {hands = "aerlizardgloves1"}},
	equipment_slot = "arms",
	icon = "lizardglove1",
	mass = 4,
	model = "lizardgloves1",
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
		["aer/kraken/wyrm"] = {upper1 = "aerleatherchest1"},["aermale"] = {upper = "aermale1leatherchest2"}},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "leather brigand chest",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "aerleatherchest1"},["aermale"] = {upper = "aermale1leatherchest1"}},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "round shield",
	icon = "woodshield1",
	model = "shield1",
	animation_hold = "hold shield",
	armor_class = 0.08,
	categories = {"shield"},
	crafting_materials = {["log"] = 3, ["iron ingot"] = 3},
	mass = 24,
	equipment_slot = "hand.L"}

Itemspec{
	name = "wooden shield",
	icon = "woodshield2",
	model = "shield-001-wood1",
	animation_hold = "hold shield",
	armor_class = 0.08,
	categories = {"shield"},
	crafting_materials = {["log"] = 1},
	equipment_slot = "hand.L",
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "shirt",
	model = "book-000",
	icon = "cloth1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 1},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aermale"] = {upper = "aermale1shirt1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "pants",
	icon = "cloth1",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 1},
	effect_craft = "craftsewing1",
	equipment_models = {
		["aermale"] = {legs = "aermale1pants1"}},
	equipment_slot = "lowerbody",
	mass = 1,
	water_gravity = Vector(0,6,0)}
