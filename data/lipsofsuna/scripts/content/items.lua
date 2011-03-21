------------------------------------------------------------------------------
-- Materials.

Itemspec{
	name = "adamantium",
	model = "adamantium1",
	icon = "stone2",
	mass = 3,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "aquanite stone",
	model = "aquanitestone1",
	icon = "stone2",
	mass = 3,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "basalt stone",
	model = "basaltstone1",
	icon = "stone1",
	mass = 3,
	categories = {"build", "material"},
	construct_tile = "basalt1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "berry",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "crimson stone",
	model = "crimsonstone1",
	icon = "stone2",
	mass = 3,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "dragon scale",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "ferrostone",
	model = "ferrostone1",
	icon = "stone2",
	mass = 5,
	categories = {"build", "material"},
	construct_tile = "ferrostone1",
	construct_tile_count = 1,
	crafting_materials = {["granite stone"] = 2},
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "flax",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "fruit",
	model = "book-000", -- FIXME
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
	mass = 3,
	stacking = true}

Itemspec{
	name = "granite stone",
	model = "granitestone1",
	icon = "stone1",
	categories = {"build", "material"},
	construct_tile = "granite1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	health = 50,
	mass = 3,
	stacking = true}

Itemspec{
	name = "iron ingot",
	model = "book-000", -- FIXME
	icon = "ingot1",
	mass = 5,
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
	name = "lizard scale",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "log",
	model = "log1",
	icon = "log1",
	categories = {"build", "material","throwable"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	construct_tile = "wood1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 50,
	mass = 3,
	stacking = true,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "another log",
	model = "log2",
	icon = "log1",
	categories = {"build", "material","throwable"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	construct_tile = "wood1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 50,
	mass = 3,
	stacking = true,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "milky powder",
	model = "book-000", -- FIXME
	icon = "mushroom1",
	categories = {"material"},
	crafting_count = 20,
	crafting_materials = {["mushroom"] = 1},
	mass = 1,
	stacking = true}

Itemspec{
	name = "mushroom",
	model = "mushroom-000",--"mushroom-001"..."mushroom-008"
	icon = "mushroom1",
	categories = {"material"},
	mass = 1,
	stacking = true}

Itemspec{
	name = "sand chunk",
	model = "sandchunk1",
	categories = {"build", "material"},
	construct_tile = "sand1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	health = 50,
	mass = 3,
	stacking = true}

Itemspec{
	name = "soil chunk",
	model = "soilchunk1",
	categories = {"build", "material"},
	construct_tile = "soil1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	health = 50,
	mass = 3,
	stacking = true}

Itemspec{
	name = "wheat",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

------------------------------------------------------------------------------
-- Weapons.

Itemspec{
	name = "adamantium crossbow",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	damage = 30,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "adamantium dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	damage = 20,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "adamantium hatchet",
	model = "axe1",
	icon = "axe1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	damage = 20,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "adamantium mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	damage = 20,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "adamantium musket",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 15, ["log"] = 2},
	damage = 50,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "adamantium sword",
	model = "sword1",
	icon = "sword1",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	damage = 40,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite crossbow",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	damage = 22,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage = 15,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite grenade",
	model = "grenade1",
	mass = 3,
	animation_attack = "throw grenade",
	animation_hold = "hold grenade",
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["aquanite stone"] = 1, ["iron ingot"] = 1},
	damage = 40,
	damage_bonus_intelligence = 0.2,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	mass = 5,
	stacking = true}

Itemspec{
	name = "aquanite hatchet",
	model = "axe1",
	icon = "axe1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage = 15,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	damage = 15,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite musket",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 15, ["log"] = 2},
	damage = 37,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite sword",
	model = "sword1",
	icon = "sword1",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage = 30,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "crimson crossbow",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	damage = 22,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "crimson dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage = 15,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "crimson grenade",
	model = "grenade1",
	mass = 3,
	animation_attack = "throw grenade",
	animation_hold = "hold grenade",
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["crimson stone"] = 1, ["iron ingot"] = 1},
	damage = 40,
	damage_bonus_intelligence = 0.2,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	mass = 5,
	stacking = true}

Itemspec{
	name = "crimson hatchet",
	model = "axe1",
	icon = "axe1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage = 15,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "crimson mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	damage = 15,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "crimson musket",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 15, ["log"] = 2},
	damage = 37,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "crimson sword",
	model = "sword1",
	icon = "sword1",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage = 30,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "iron crossbow",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	damage = 15,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "iron dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage = 10,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R",
	mass = 5}

Itemspec{
	name = "iron hatchet",
	model = "axe1",
	icon = "axe1",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 2},
	damage = 10,
	damage_bonus_strength = 0.2,
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	equipment_slot = "hand.R",
	mass = 10}

Itemspec{
	name = "iron mattock",
	model = "mattock-000",
	icon = "mattock1",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	damage = 10,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "iron musket",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 5, ["log"] = 2},
	damage = 25,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	mass = 20}

Itemspec{
	name = "iron sword",
	model = "sword1",
	icon = "sword1",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	damage = 10,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "wooden crossbow",
	model = "bow1", -- FIXME
	icon = "bow1",
	animation_attack = "attack bow",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 3},
	damage = 7,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	water_gravity = Vector(0,1,0),
	mass = 7}

Itemspec{
	name = "wooden dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 5,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R",
	water_gravity = Vector(0,1,0),
	mass = 7}

Itemspec{
	name = "wooden hatchet",
	model = "axe1",
	icon = "axe1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 5,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	water_gravity = Vector(0,1,0),
	mass = 7}

Itemspec{
	name = "wooden mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["log"] = 3},
	damage = 5,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R",
	water_gravity = Vector(0,1,0),
	mass = 7}

Itemspec{
	name = "wooden sword",
	model = "sword1",
	icon = "sword1",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 10,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	water_gravity = Vector(0,1,0),
	mass = 7}

Itemspec{
	name = "wooden club",
	model = "woodenclub1",
	icon = "mace1",
	animation_hold = "hold blunt",
	animation_attack = "attack blunt",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 15,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	water_gravity = Vector(0,1,0),
	mass = 7}

Itemspec{
	name = "wooden staff",
	model = "spear1",
	icon = "staff1",
	animation_hold = "hold staff",
	animation_attack = "attack staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 15,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	armor_class = 0.04,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,1,0),
	mass = 7}

Itemspec{
	name = "luminous staff",
	model = "staff1",
	icon = "staff2",
	animation_hold = "hold staff",
	animation_attack = "attack staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 15,
	damage_bonus_perception = 0.2,
	damage_bonus_willpower = 0.2,
	armor_class = 0.04,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	special_effects = {
		{slot = "flame", model = "torchfx1"},{slot = "flame2", model = "torchfx1"}},
	water_gravity = Vector(0,1,0),
	mass = 7}

------------------------------------------------------------------------------
-- Armor.

-- TODO

Itemspec{
	name = "dress",
	model = "chinadress1",
	icon = "cloth1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {log = 1},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "aerchinadress1", panties = "panties2"},
		["android"] = {upper1 = "androidchinadress1"},
		["devora"] = {upper1 = "devorachinadress1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "trog rags",
	model = "book-000",
	icon = "cloth1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {log = 1},
	equipment_models = {
		["troglodyte female"] = {upper1 = "trogfemrags1"}},
	equipment_slot = "upperbody",
	mass = 1,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "iron greaves",
	model = "book-000",
	icon = "metalboot1",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 3},
	equipment_models = {
		["aer/kraken/wyrm"] = {legs = "aerlegplate1"}},
	equipment_slot = "feet",
	mass = 15}

Itemspec{
	name = "iron gauntlets",
	model = "book-000",
	icon = "metalglove1",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 2},
	equipment_models = {
		["aer/kraken/wyrm"] = {arms = "aerarmplate1"}},
	equipment_slot = "arms",

	mass = 10}

Itemspec{
	name = "iron breastplate",
	model = "book-000",
	icon = "metalarmor1",
	armor_class = 0.18,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 5},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper = "aerupperplate1", lower = "aerlowerplate1"}},
	equipment_slot = "upperbody",
	mass = 25}

Itemspec{
	name = "kitty headband",
	model = "headband1",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_materials = {hide = 2},
	equipment_models = {
		["aer/android/kraken/wyrm"] = {headband = "headband1"}},
	equipment_slot = "head",
	mass = 1,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather helm",
	model = "book-000",
	icon = "leatherhelm1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 2},
	equipment_models = {
		["aermale"] = {hair = "aermale1leatherhead2"}},
	equipment_slot = "head",
	mass = 3,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather leggings",
	model = "leggings1",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {feet = false, legs = "aerleggings1"}},
	equipment_slot = "feet",
	mass = 5,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather gloves",
	model = "book-000",
	icon = "leatherglove1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {["hide"] = 2},
	equipment_models = {
		["aermale"] = {hands = "aermale1leatherarms2"}},
	equipment_slot = "arms",
	mass = 3,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather boots",
	model = "boots1",
	icon = "leatherboot1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/android/kraken/wyrm"] = {legs = "aerlegarmor2", legs1 = "boots1"},["aermale"] = {feet = "aermale1boots1"}},
	equipment_slot = "feet",
	mass = 3,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather pants",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {skirt = "aerlowerarmor1", panties = "panties2"},["aermale"] = {legs = "aermale1leatherlegs2"}},
	equipment_slot = "lowerbody",
	mass = 5,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather brigand pants",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {skirt = "aerlowerarmor1", panties = "panties2"},["aermale"] = {legs = "aermale1leatherlegs1"}},
	equipment_slot = "lowerbody",
	mass = 5,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather top",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "bra1"}},
	equipment_slot = "upperbody",
	mass = 5,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather skirt",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {skirt = "aerleatherskirt1", panties = "panties2"}},
	equipment_slot = "lowerbody",
	mass = 5,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "lizard boots",
	model = "boots2",
	icon = "lizardboot1",
	armor_class = 0.5,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 4},
	equipment_models = {
		["aer/android/kraken/wyrm"] = {legs1 = "aerboots2"}},
	equipment_slot = "feet",
	mass = 4,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "lizard dress",
	icon = "lizardarmor1",
	model = "book-000",
	armor_class = 0.1,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 6},
	equipment_models = {
		["aer/kraken/wyrm"] = {dress1 = "aerdress2", panties = "panties2"}},
	equipment_slot = "upperbody",
	mass = 7,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "lizard gloves",
	model = "gloves2",
	icon = "lizardglove1",
	armor_class = 0.5,
	categories = {"armor"},
	crafting_materials = {["lizard scale"] = 4},
	equipment_models = {
		["aer/android/kraken/wyrm"] = {arms1 = "aergloves2"}},
	equipment_slot = "arms",
	mass = 4,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather chest",
	model = "book-000",
	icon = "leatherarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "aerleatherchest1"},["aermale"] = {upper = "aermale1leatherchest2"}},
	equipment_slot = "upperbody",
	icon = "cloth1",
	mass = 5,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "leather brigand chest",
	model = "book-000",
	icon = "lizardarmor1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 4},
	equipment_models = {
		["aer/kraken/wyrm"] = {upper1 = "aerleatherchest1"},["aermale"] = {upper = "aermale1leatherchest1"}},
	equipment_slot = "upperbody",
	icon = "cloth1",
	mass = 5,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "round shield",
	icon = "shield1",
	model = "shield1",
	animation_hold = "hold shield",
	armor_class = 0.08,
	categories = {"shield"},
	crafting_materials = {["log"] = 3, ["iron ingot"] = 3},
	mass = 24,
	equipment_slot = "hand.L"}

Itemspec{
	name = "wooden shield",
	icon = "shield1",
	model = "shield-001-wood1",
	animation_hold = "hold shield",
	armor_class = 0.08,
	categories = {"shield"},
	crafting_materials = {["log"] = 1},
	equipment_slot = "hand.L",
	mass = 7,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "shirt",
	model = "book-000",
	icon = "cloth1",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 1},
	equipment_models = {
		["aermale"] = {upper = "aermale1shirt1"}},
	equipment_slot = "upperbody",
	icon = "cloth1",
	mass = 1,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "pants",
	icon = "cloth1",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_materials = {hide = 1},
	equipment_models = {
		["aermale"] = {legs = "aermale1pants1"}},
	equipment_slot = "lowerbody",
	icon = "cloth1",
	mass = 1,
	water_gravity = Vector(0,1,0)}

------------------------------------------------------------------------------
-- Miscellaneous.

Itemspec{
	name = "arrow",
	model = "arrow-000",
	categories = {"arrow"},
	damage = 5,
	crafting_count = 10,
	crafting_materials = {["log"] = 1},
	icon = "arrow1",
	mass = 1,
	stacking = true,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "bullet",
	model = "arrow-000", -- FIXME
	mass = 3,
	categories = {"bullet"},
	damage = 5,
	crafting_count = 10,
	crafting_materials = {["iron ingot"] = 1, ["crimson stone"] = 3},
	icon = "arrow1",
	stacking = true}

Itemspec{
	name = "torch",
	model = "torch1",
	animation_hold = "hold torch",
	categories = {"misc"},
	crafting_materials = {["log"] = 1},
	equipment_slot = "hand.L",
	icon = "torch1",
	mass = 3,
	special_effects = {
		{slot = "flame", model = "torchfx1"}},
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "impstone",
	model = "firestone1",
	icon = "stone1",
	equipment_slot = "hand.R",
	categories = {"throwable", "weapon"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	damage = 5,
	mass = 4,
	stacking = false,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "firestone",
	icon = "stone2",
	model = "firestone1",
	categories = {"explosive", "throwable", "weapon"},
	animation_hold = "hold bulky",
	animation_attack = "throw bulky",
	damage = 40,
	damage_bonus_intelligence = 0.2,
	destroy_actions = {"explode"},
	destroy_timer = 5,
	equipment_slot = "hand.R",
	health = 1,
	mass = 4,
	special_effects = {
		{slot = "flame", model = "torchfx1"}},
	stacking = false}

------------------------------------------------------------------------------
-- Containers.

Itemspec{
	name = "chest",
	model = "chest1",
	mass = 100,
	animations = {
		["loot"] = {animation = "opening", channel = 1, permanent = true, repeat_start = 10},
		["looted"] = {animation = "open", channel = 1, permanent = true, repeat_start = 10, time = 10}},
	categories = {"container", "generate"},
	crafting_materials = {log = 1},
	inventory_size = 4,
	inventory_type = "chest",
	loot_categories = {"armor", "book", "material", "misc", "potion", "shield", "weapon"},
	loot_count = {2, 4},
	stacking = false,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "jar",
	model = "jar1",
	mass = 10,
	categories = {"container", "generate"},
	crafting_materials = {["granite stone"] = 3},
	inventory_size = 4,
	inventory_type = "chest",
	loot_categories = {"material", "misc", "potion"},
	loot_count = {0, 4},
	stacking = false}

Itemspec{
	name = "workbench",
	model = "workbench1",
	icon = "workbench1",
	mass = 30,
	animation_looted = "open",
	animation_looting = "opening",
	categories = {"container", "workbench"},
	crafting_materials = {log = 1},
	inventory_size = 4,
	inventory_type = "workbench",
	stacking = false,
	water_gravity = Vector(0,1,0)}

------------------------------------------------------------------------------
-- Decorations

Itemspec{
	name = "chair",
	model = "chair1",
	mass = 10,
	categories = {"furniture"},
	crafting_materials = {log = 5},
	health = 30,
	stacking = false,
	water_gravity = Vector(0,1,0)}

------------------------------------------------------------------------------
-- FIXME

Itemspec{
	name = "air tank",
	model = "airtank1",
	mass = 15,
	categories = {"explosive"},
	crafting_materials = {log = 1},
	destroy_actions = {"explode"},
	health = 1,
	stacking = false,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "boomerang",
	animations = {
		["fly start"] = {animation = "fly", channel = 1, permanent = true},
		["fly stop"] = {channel = 1, permanent = true}},
	mass = 3,
	model = "boomerang1",
	categories = {"boomerang", "throwable", "weapon"},
	crafting_materials = {log = 1},
	damage = 20,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	stacking = true,
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "gem",
	icon = "gem1",
	model = "gem-000",
	mass = 2,
	categories = {"reagent"},
	stacking = true}

Itemspec{
	name = "greater health potion",
	model = "potion3",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 75},
	stacking = true,
	icon = "potion1",
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "greater mana potion",
	model = "potion7",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 75},
	stacking = true,
	icon = "potion2",
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "health potion",
	model = "potion3",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 50},
	stacking = true,
	icon = "potion1",
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "lesser health potion",
	model = "potion3",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 25},
	stacking = true,
	icon = "potion1",
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "lesser mana potion",
	model = "potion7",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 25},
	stacking = true,
	icon = "potion2",
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "mana potion",
	model = "potion7",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 50},
	stacking = true,
	icon = "potion2",
	water_gravity = Vector(0,1,0)}

Itemspec{
	name = "Steamworks",
	icon = "scroll1",
	model = "book-000",
	mass = 2,
	categories = {"book"},
	book_text =
[[Steam-powered devices have brought a world of new opportunities to the aer society. The technology is still young but it has shown lots of promise in automating tasks. There is even progress in creating advanced devices that are able to reason and act on their own.

Steam technology relies on mechanical parts that are brought into motion by steam circulating in a system of pipes. The steam was initially created by conventional heating methods but more compact and efficient methods based on elemental forces such as ki were discovered very quickly.

With the power of this new technology on their side, the aer society are confident that they can defeat the unknown forces behind the withering of the World Tree.]]}
