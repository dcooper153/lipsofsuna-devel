------------------------------------------------------------------------------
-- Materials.

Itemspec{
	name = "adamantium",
	model = "adamantium1",
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "aquanite stone",
	model = "aquanitestone1",
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "basalt stone",
	model = "basaltstone1",
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "basalt1",
	construct_tile_count = 3,
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
	mass = 1,
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
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "ferrostone1",
	construct_tile_count = 3,
	crafting_materials = {["granite stone"] = 3},
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
	mass = 3,
	categories = {"material"},
	crafting_materials = {["sand chunk"] = 3},
	stacking = true}

Itemspec{
	name = "granite stone",
	model = "granitestone1",
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "granite1",
	construct_tile_count = 3,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "iron ingot",
	model = "book-000", -- FIXME
	mass = 3,
	categories = {"build", "material"},
	construct_tile = "iron1",
	construct_tile_count = 4,
	crafting_materials = {["ferrostone"] = 3},
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "hide",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"material"},
	stacking = true}

Itemspec{
	name = "log",
	model = "book-000", -- FIXME
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "wood1",
	construct_tile_count = 4,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "mushroom",
	model = "mushroom-000",--"mushroom-001"..."mushroom-008"
	mass = 1,
	categories = {"material"},
	stacking = true,
	icon = "mushroom1"}

Itemspec{
	name = "sand chunk",
	model = "sandchunk1",
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "sand1",
	construct_tile_count = 3,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "soil chunk",
	model = "soilchunk1",
	mass = 1,
	categories = {"build", "material"},
	construct_tile = "soil1",
	construct_tile_count = 3,
	equipment_slot = "hand.R",
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
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	damage = 30,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "adamantium dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	damage = 20,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "adamantium hatchet",
	model = "axe-000",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	damage = 20,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "adamantium mattock",
	model = "mattock-000",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	damage = 20,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "adamantium musket",
	model = "bow1", -- FIXME
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 15, ["log"] = 2},
	damage = 50,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "adamantium sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	damage = 40,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "aquanite crossbow",
	model = "bow1", -- FIXME
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	damage = 22,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "aquanite dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage = 15,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "aquanite grenade",
	model = "grenade1",
	mass = 3,
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["aquanite stone"] = 1, ["iron ingot"] = 1},
	damage = 40,
	damage_bonus_intelligence = 0.2,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	stacking = true}

Itemspec{
	name = "aquanite hatchet",
	model = "axe-000",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage = 15,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "aquanite mattock",
	model = "mattock-000",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	damage = 15,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "aquanite musket",
	model = "bow1", -- FIXME
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 15, ["log"] = 2},
	damage = 37,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "aquanite sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage = 30,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "crimson crossbow",
	model = "bow1", -- FIXME
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	damage = 22,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "crimson dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage = 15,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "crimson grenade",
	model = "grenade1",
	mass = 3,
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["crimson stone"] = 1, ["iron ingot"] = 1},
	damage = 40,
	damage_bonus_intelligence = 0.2,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	stacking = true}

Itemspec{
	name = "crimson hatchet",
	model = "axe-000",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage = 15,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "crimson mattock",
	model = "mattock-000",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	damage = 15,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "crimson musket",
	model = "bow1", -- FIXME
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 15, ["log"] = 2},
	damage = 37,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "crimson sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage = 30,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "iron crossbow",
	model = "bow1", -- FIXME
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	damage = 15,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "iron dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage = 10,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "iron hatchet",
	model = "axe-000",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage = 10,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "iron mattock",
	model = "mattock-000",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	damage = 10,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "iron musket",
	model = "bow1", -- FIXME
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 5, ["log"] = 2},
	damage = 25,
	damage_bonus_intelligence = 0.2,
	damage_bonus_perception = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "iron sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage = 10,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "wooden crossbow",
	model = "bow1", -- FIXME
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 3},
	damage = 7,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "wooden dagger",
	model = "sword1", -- FIXME
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 5,
	damage_bonus_dexterity = 0.4,
	equipment_slot = "hand.R"}

Itemspec{
	name = "wooden hatchet",
	model = "axe-000",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 5,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

Itemspec{
	name = "wooden mattock",
	model = "mattock-000",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["log"] = 3},
	damage = 5,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "wooden sword",
	model = "sword1",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 1},
	damage = 10,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

------------------------------------------------------------------------------
-- Armor.

-- TODO

Itemspec{
	name = "dress",
	model = "book-000",
	armor_class = 0.02,
	categories = {"armor"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	equipment_models = {upper1 = "chinadress1", panties = "panties2"},
	equipment_slot = "upperbody",
	icon = "cloth1"}

Itemspec{
	name = "iron greaves",
	model = "book-000",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 3},
	equipment_models = {legs = "aerlegplate1"},
	equipment_slot = "feet"}

Itemspec{
	name = "iron gauntlets",
	model = "book-000",
	armor_class = 0.08,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 2},
	equipment_models = {arms = "aerarmplate1"},
	equipment_slot = "arms"}

Itemspec{
	name = "iron breastplate",
	model = "book-000",
	armor_class = 0.18,
	categories = {"armor"},
	crafting_materials = {["iron ingot"] = 5},
	equipment_models = {upper = "aerupperplate1", lower = "aerlowerplate1"},
	equipment_slot = "upperbody"}

Itemspec{
	name = "leather leggings",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	equipment_models = {legs = "aerlegarmor1"},
	equipment_slot = "feet"}

Itemspec{
	name = "leather pants",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	equipment_models = {skirt = "aerlowerarmor1", panties = "panties2"},
	equipment_slot = "lowerbody"}

Itemspec{
	name = "leather top",
	model = "book-000",
	armor_class = 0.04,
	categories = {"armor"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	equipment_models = {upper1 = "bra1"},
	equipment_slot = "upperbody",
	icon = "cloth1"}

Itemspec{
	name = "wooden shield",
	model = "shield-000",
	armor_class = 0.08,
	categories = {"shield"},
	crafting_materials = {["log"] = 1},
	equipment_slot = "hand.L"}

------------------------------------------------------------------------------
-- Miscellaneous.

Itemspec{
	name = "arrow",
	model = "arrow-000",
	mass = 1,
	categories = {"arrow"},
	damage = 5,
	crafting_count = 10,
	crafting_materials = {["log"] = 1},
	icon = "arrow1",
	stacking = true}

Itemspec{
	name = "bullet",
	model = "arrow-000", -- FIXME
	mass = 1,
	categories = {"bullet"},
	damage = 5,
	crafting_count = 10,
	crafting_materials = {["iron ingot"] = 1, ["crimson stone"] = 3},
	icon = "arrow1",
	stacking = true}

Itemspec{
	name = "torch",
	model = "torch1",
	categories = {"misc"},
	crafting_materials = {["log"] = 1},
	equipment_slot = "hand.L",
	icon = "torch1"}

------------------------------------------------------------------------------
-- FIXME

Itemspec{
	name = "air tank",
	model = "airtank1",
	mass = 15,
	categories = {"explosive"},
	crafting_materials = {log = 1},
	crafting_tools = {saw = 1},
	destroy_actions = {"explode"},
	health = 1,
	stacking = false}

Itemspec{
	name = "boomerang",
	model = "boomerang1",
	mass = 3,
	categories = {"boomerang", "throwable", "weapon"},
	crafting_materials = {log = 1},
	crafting_tools = {saw = 1},
	damage = 20,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	stacking = true}

Itemspec{
	name = "gem",
	model = "gem-000",
	mass = 2,
	categories = {"reagent"},
	stacking = true}

Itemspec{
	name = "greater health potion",
	model = "potion-002",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 75},
	crafting_tools = {["mortar and pestle"] = 1},
	stacking = true,
	icon = "potion1"}

Itemspec{
	name = "greater mana potion",
	model = "potion-005",--"potion-006"
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 75},
	crafting_tools = {["mortar and pestle"] = 1},
	stacking = true,
	icon = "potion2"}

Itemspec{
	name = "health potion",
	model = "potion-001",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 50},
	crafting_tools = {["mortar and pestle"] = 1},
	stacking = true,
	icon = "potion1"}

Itemspec{
	name = "lesser health potion",
	model = "potion-000",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 25},
	crafting_tools = {["mortar and pestle"] = 1},
	stacking = true,
	icon = "potion1"}

Itemspec{
	name = "lesser mana potion",
	model = "potion-003",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 25},
	crafting_tools = {["mortar and pestle"] = 1},
	stacking = true,
	icon = "potion2"}

Itemspec{
	name = "mana potion",
	model = "potion-004",
	mass = 2,
	categories = {"potion"},
	crafting_materials = {mushroom = 1},
	crafting_skills = {intelligence = 50},
	crafting_tools = {["mortar and pestle"] = 1},
	stacking = true,
	icon = "potion2"}

Itemspec{
	name = "mortar and pestle",
	model = "alchemy-000",
	categories = {"tool"}}

Itemspec{
	name = "saw",
	model = "saw1",
	categories = {"tool"}}

Itemspec{
	name = "Steamworks",
	model = "book-000",
	mass = 2,
	categories = {"book"},
	book_text =
[[Steam-powered devices have brought a world of new opportunities to the aer society. The technology is still young but it has shown lots of promise in automating tasks. There is even progress in creating advanced devices that are able to reason and act on their own.

Steam technology relies on mechanical parts that are brought into motion by steam circulating in a system of pipes. The steam was initially created by conventional heating methods but more compact and efficient methods based on elemental forces such as ki were discovered very quickly.

With the power of this new technology on their side, the aer society are confident that they can defeat the unknown forces behind the withering of the World Tree.]]}
