-- (materials sorted by durability)
-- weapon \ material	W  S  Q  I  C  A
-- ======================================
-- mattock		+  +  +  +  +  +
-- staff		+  -  -  -  -  -
--
-- dagger		+  +  +  +  +  +
-- hatchet		+  +  +  +  +  +
-- sword		+  +  +  +  +  +
-- club 		+  -  -  -  -  -
--
-- grenade		-  -  +  -  +  -
--
-- crossbow		+  -  +  +  +  +
-- musket		-  -  +  +  +  +

Itemspec{
	name = "adamantium crossbow",
	model = "bow1", -- FIXME
	icon = "crossbow2",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 9},
	influences_base = {physical = -15},
	influences_bonus = {dexterity = 0.01},
	animation_hold = "hold crossbow",
	animation_attack = "attack crossbow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "adamantium dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	influences_base = {physical = -10},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "adamantium hatchet",
	model = "axe1",
	icon = "axe3",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	influences_base = {hatchet = -10, physical = -10},
	influences_bonus = {strength = 0.01},
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
	influences_base = {physical = -10},
	influences_bonus = {strength = 0.01},
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "adamantium musket",
	model = "bow1", -- FIXME
	icon = "musket1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["adamantium"] = 15, ["log"] = 2},
	influences_base = {physical = -25},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	animation_hold = "hold crossbow",
	animation_attack = "attack crossbow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "adamantium sword",
	model = "sword1",
	icon = "sword3",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["adamantium"] = 3},
	influences_base = {physical = -20},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	equipment_slot = "hand.R",
	mass = 15}

Itemspec{
	name = "aquanite crossbow",
	model = "bow1", -- FIXME
	icon = "crossbow1",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	influences_base = {cold = -6, physical = -6},
	influences_bonus = {dexterity = 0.01},
	animation_hold = "hold crossbow",
	animation_attack = "attack crossbow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "aquanite dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	influences_base = {cold = -4, physical = -4},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	health = 500,
	mass = 15}

Itemspec{
	name = "aquanite grenade",
	model = "grenade1",
	icon = "grenade1",
	mass = 3,
	animation_attack = "throw grenade",
	animation_hold = "hold grenade",
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["aquanite stone"] = 1, ["iron ingot"] = 1},
	damage_mining = 1,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	influences_base = {cold = -15, physical = -5},
	influences_bonus = {intelligence = 0.01},
	mass = 5,
	stacking = true}

Itemspec{
	name = "aquanite hatchet",
	model = "axe1",
	icon = "axe2",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	influences_base = {cold = -4, hatchet = -7, physical = -4},
	influences_bonus = {strength = 0.01},
	health = 500,
	mass = 15}

Itemspec{
	name = "aquanite mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 9},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {cold = -3, physical = -4},
	influences_bonus = {strength = 0.01},
	mass = 15}

Itemspec{
	name = "aquanite musket",
	model = "bow1", -- FIXME
	icon = "musket1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["aquanite stone"] = 15, ["log"] = 2},
	animation_hold = "hold crossbow",
	animation_attack = "attack crossbow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	influences_base = {cold = -9, physical = -9},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	mass = 15}

Itemspec{
	name = "aquanite sword",
	model = "sword5",
	icon = "sword2",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["aquanite stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {cold = -8, physical = -8},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 15}

Itemspec{
	name = "boomerang",
	animations = {
		["fly start"] = {animation = "fly", channel = 1, permanent = true},
		["fly stop"] = {channel = 1, permanent = true}},
	mass = 2,
	model = "boomerang1",
	icon = "boomerang1",
	categories = {"boomerang", "throwable", "weapon"},
	crafting_materials = {log = 2},
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.01},
	equipment_slot = "hand.R",
	stacking = true,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "crimson crossbow",
	model = "bow1", -- FIXME
	icon = "crossbow1",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	animation_hold = "hold crossbow",
	animation_attack = "attack crossbow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	influences_base = {fire = -6, physical = -6},
	influences_bonus = {dexterity = 0.01},
	mass = 15}

Itemspec{
	name = "crimson dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {fire = -4, physical = -4},
	influences_bonus = {dexterity = 0.01},
	mass = 15}

Itemspec{
	name = "crimson grenade",
	model = "grenade1",
	icon = "grenade1",
	mass = 3,
	animation_attack = "throw grenade",
	animation_hold = "hold grenade",
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {["crimson stone"] = 1, ["iron ingot"] = 1},
	influences_base = {fire = -15, physical = -5},
	influences_bonus = {intelligence = 0.01},
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	mass = 5,
	stacking = true}

Itemspec{
	name = "crimson hatchet",
	model = "axe1",
	icon = "axe2",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {fire = -4, hatchet = -7, physical = -4},
	influences_bonus = {strength = 0.01},
	mass = 15}

Itemspec{
	name = "crimson mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["crimson stone"] = 9},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {fire = -4, physical = -4},
	influences_bonus = {strength = 0.01},
	mass = 15}

Itemspec{
	name = "crimson musket",
	model = "bow1", -- FIXME
	icon = "musket1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["crimson stone"] = 15, ["log"] = 2},
	influences_base = {fire = -8, physical = -8},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	animation_hold = "hold crossbow",
	animation_attack = "attack crossbow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "crimson sword",
	model = "sword1",
	icon = "sword2",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["crimson stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 500,
	influences_base = {fire = -8, physical = -8},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 15}

Itemspec{
	name = "iron crossbow",
	model = "bow1", -- FIXME
	icon = "crossbow1",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	influences_base = {physical = -8},
	influences_bonus = {dexterity = 0.005},
	animation_hold = "hold crossbow",
	animation_attack = "attack crossbow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	mass = 15}

Itemspec{
	name = "iron dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 1},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.01},
	mass = 5}

Itemspec{
	name = "iron hatchet",
	model = "axe1",
	icon = "axe2",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 2},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {hatchet = -5, physical = -5},
	influences_bonus = {strength = 0.01},
	mass = 10}

Itemspec{
	name = "iron mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {physical = -5},
	influences_bonus = {strength = 0.01},
	mass = 15}

Itemspec{
	name = "iron musket",
	model = "bow1", -- FIXME
	icon = "musket1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	crafting_materials = {["iron ingot"] = 5, ["log"] = 2},
	animation_hold = "hold crossbow",
	animation_attack = "attack crossbow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	influences_base = {physical = -13},
	influences_bonus = {intelligence = 0.005, perception = 0.005},
	mass = 20}

Itemspec{
	name = "iron sword",
	model = "sword1",
	icon = "sword2",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["iron ingot"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 220,
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 15}

Itemspec{
	name = "stone dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["granite stone"] = 2},
	damage_mining = 1,
	equipment_slot = "hand.R",
	influences_base = {physical = -3},
	influences_bonus = {dexterity = 0.005},
	health = 130,
	mass = 6}

Itemspec{
	name = "stone hatchet",
	model = "stoneaxe1",
	icon = "axe1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["granite stone"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	influences_base = {hatchet = -3, physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 9}

Itemspec{
	name = "stone mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["granite stone"] = 4},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	influences_base = {physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 12}

Itemspec{
	name = "stone sword",
	model = "sword1",
	icon = "sword1",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["granite stone"] = 5},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 130,
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.002, strength = 0.005},
	mass = 15}

Itemspec{
	name = "wooden bow",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 5},
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.01},
	animation_hold = "hold bow",
	animation_attack = "attack bow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,6,0),
	mass = 7}
Itemspec{
	name = "fire bow",
	model = "bow1", -- FIXME
	icon = "bow2",
	ammo_type = "fire arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 5},
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.01},
	animation_hold = "hold bow",
	animation_attack = "attack bow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,6,0),
	mass = 7}

Itemspec{
	name = "explosive bow",
	model = "bow1", -- FIXME
	icon = "bow2",
	ammo_type = "explosive arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 5},
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.01},
	animation_hold = "hold bow",
	animation_attack = "attack bow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,6,0),
	mass = 7}

Itemspec{
	name = "wooden crossbow",
	model = "bow1", -- FIXME
	icon = "bow1",
	ammo_type = "arrow",
	categories = {"ranged", "weapon"},
	crafting_materials = {["log"] = 5},
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.01},
	animation_hold = "hold crossbow",
	animation_attack = "attack crossbow",
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	water_gravity = Vector(0,6,0),
	mass = 7}


Itemspec{
	name = "wooden dagger",
	model = "sword1", -- FIXME
	icon = "dagger1",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 2},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {physical = -3},
	influences_bonus = {dexterity = 0.01},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden hatchet",
	model = "axe1",
	icon = "axe1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"hatchet", "melee", "weapon"},
	crafting_materials = {["log"] = 3},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {hatchet = -3, physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden mattock",
	model = "mattock-000",
	icon = "mattock1",
	animation_hold = "hold axe",
	animation_attack = "attack axe",
	categories = {"mattock", "melee", "weapon"},
	crafting_materials = {["log"] = 4},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {physical = -3},
	influences_bonus = {strength = 0.01},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden sword",
	model = "sword1",
	icon = "sword1",
	animation_hold = "hold sword",
	animation_attack = "attack sword",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 5},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {physical = -5},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden club",
	model = "woodenclub1",
	icon = "mace1",
	animation_hold = "hold blunt",
	animation_attack = "attack blunt",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 4},
	damage_mining = 1,
	equipment_slot = "hand.R",
	health = 100,
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wooden staff",
	model = "spear1",
	icon = "staff1",
	animation_hold = "hold staff",
	animation_attack = "attack staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 5},
	damage_mining = 1,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 100,
	influences_base = {physical = -4},
	influences_bonus = {dexterity = 0.005, strength = 0.005},
	mass = 7,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "luminous staff",
	model = "staff1",
	icon = "staff2",
	animation_hold = "hold staff",
	animation_attack = "attack staff",
	categories = {"melee", "weapon"},
	crafting_materials = {["log"] = 5, ["crimson stone"] = 2},
	damage_mining = 1,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {"hand.L"},
	health = 300,
	influences_base = {fire = -2, physical = -4},
	influences_bonus = {perception = 0.005, willpower = 0.005},
	mass = 7,
	special_effects = {
		{slot = "flame", model = "torchfx1"},
		{slot = "flame2", model = "torchfx1"}},
	water_gravity = Vector(0,6,0)}
