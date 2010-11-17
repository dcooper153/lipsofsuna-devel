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
	name = "arrow",
	model = "arrow-000",
	mass = 2,
	categories = {"arrow"},
	damage = 5,
	crafting_count = 10,
	crafting_materials = {log = 1},
	crafting_tools = {saw = 1},
	icon = "arrow1",
	stacking = true}

Itemspec{
	name = "axe",
	model = "axe-000",
	categories = {"melee", "weapon"},
	damage = 10,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R",
	icon = "axe1"}

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
	name = "bow",
	model = "bow1",
	categories = {"ranged", "weapon"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	damage = 10,
	damage_bonus_dexterity = 0.2,
	equipment_slot = "hand.R",
	icon = "bow1"}

Itemspec{
	name = "chainmail leggings",
	model = "book-000",
	categories = {"armor"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	equipment_models = {legs = "aerlegarmor1"},
	equipment_slot = "feet"}

Itemspec{
	name = "chainmail pants",
	model = "book-000",
	categories = {"armor"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	equipment_models = {skirt = "aerlowerarmor1", panties = "panties2"},
	equipment_slot = "lowerbody"}

Itemspec{
	name = "chainmail tunic",
	model = "book-000",
	categories = {"armor"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	equipment_models = {upper1 = "bra1"},
	equipment_slot = "upperbody",
	icon = "cloth1"}

Itemspec{
	name = "dress",
	model = "book-000",
	categories = {"armor"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	equipment_models = {upper1 = "chinadress1", panties = "panties2"},
	equipment_slot = "upperbody",
	icon = "cloth1"}

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
	icon = "potion2"}

Itemspec{
	name = "grenade",
	model = "grenade1",
	mass = 3,
	categories = {"explosive", "throwable", "weapon"},
	crafting_materials = {log = 1},
	crafting_tools = {saw = 1},
	damage = 30,
	damage_bonus_intelligence = 0.2,
	destroy_actions = {"explode"},
	destroy_timer = 3,
	equipment_slot = "hand.R",
	health = 1,
	stacking = true}

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
	name = "katana",
	model = "katana-000",
	categories = {"melee", "weapon"},
	damage = 10,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

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
	name = "log",
	model = "mushroom-000",
	mass = 2,
	categories = {"reagent"},
	stacking = true}

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
	name = "mattock",
	model = "mattock-000",
	categories = {"melee", "weapon"},
	crafting_tools = {saw = 1},
	crafting_materials = {log = 1},
	damage = 6,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "morning star",
	model = "morningstar-000",
	categories = {"melee", "weapon"},
	damage = 10,
	damage_bonus_strength = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "mortar and pestle",
	model = "alchemy-000",
	categories = {"tool"}}

Itemspec{
	name = "mushroom",
	model = "mushroom-000",--"mushroom-001"..."mushroom-008"
	mass = 2,
	categories = {"reagent"},
	stacking = true,
	icon = "mushroom1"}

Itemspec{
	name = "poleaxe",
	model = "poleaxe-000", --"poleaxe-001"
	categories = {"melee", "weapon"},
	damage = 10,
	equipment_slot = "hand.R"}

Itemspec{
	name = "saw",
	model = "saw1",
	categories = {"tool"}}

Itemspec{
	name = "shield",
	model = "shield-000",
	categories = {"shield"},
	damage = 10,
	equipment_slot = "hand.L"}

Itemspec{
	name = "spear",
	model = "spear-000",
	categories = {"melee", "weapon"},
	damage = 10,
	damage_bonus_dexterity = 0.2,
	damage_bonus_strength = 0.1,
	equipment_slot = "hand.R"}

Itemspec{
	name = "sword",
	model = "sword-000",
	categories = {"melee", "weapon"},
	damage = 10,
	equipment_slot = "hand.R"}

Itemspec{
	name = "torch",
	model = "torch1",
	categories = {"misc"},
	equipment_slot = "hand.L",
	icon = "torch1"}

Itemspec{
	name = "wand",
	model = "wand-000", --"wand-001"
	categories = {"melee", "weapon"},
	damage = 5,
	damage_bonus_willpower = 0.2,
	equipment_slot = "hand.R"}

Itemspec{
	name = "Steamworks",
	model = "book-000",
	mass = 2,
	categories = {"book"},
	book_text =
[[Steam-powered devices have brought a world of new opportunities to the aer society. The technology is still young but it has shown lots of promise in automating tasks. There is even progress in creating advanced devices that are able to reason and act on their own.

Steam technology relies on mechanical parts that are brought into motion by steam circulating in a system of pipes. The steam was initially created by conventional heating methods but more compact and efficient methods based on elemental forces such as ki were discovered very quickly.

With the power of this new technology on their side, the aer society are confident that they can defeat the unknown forces behind the withering of the World Tree.]]}
