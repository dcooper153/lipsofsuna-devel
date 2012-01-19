Program:add_path(Mod.path)

Species{
	name = "Foreman",
	base = "aer",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "foreman",
	marker = "foreman",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"}}

Species{
	name = "Noemi",
	base = "aer",
	dialog = "noemi",
	marker = "noemi",
	dead = true,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"good"}}

Species{
	name = "brigand",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.5,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron sword", "leather top", "leather pants", "leather leggings"},
	loot_categories = {"armor", "material", "potion", "shield", "weapon"},
	loot_count = {2, 4}}

Species{
	name = "brigand king",
	base = "aer",
	categories = {"special"},
	difficulty = 1,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	marker = "brigandking",
	inventory_items = {"adamantium sword", "iron breastplate", "iron greaves", "iron gauntlets"},
	loot_categories = {"armor", "shield", "weapon"},
	loot_count = {5, 10},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 100},
		{name = "intelligence", max = 100, val = 0},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 40},
		{name = "willpower", max = 80, val = 0}}}

Species{
	name = "novice brigand archer",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.3,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"wooden crossbow", "leather top", "leather pants", "leather leggings", ["arrow"] = 20},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count = {2, 4},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 30},
		{name = "intelligence", max = 100, val = 5},
		{name = "perception", max = 60, val = 30},
		{name = "strength", max = 40, val = 5},
		{name = "willpower", max = 80, val = 5}}}

Species{
	name = "master brigand archer",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.7,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron crossbow", "iron breastplate", "iron greaves", "iron gauntlets", ["arrow"] = 40},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count = {2, 4},
	skills = {
		{name = "dexterity", max = 80, val = 80},
		{name = "health", max = 100, val = 60},
		{name = "intelligence", max = 100, val = 5},
		{name = "perception", max = 60, val = 40},
		{name = "strength", max = 40, val = 5},
		{name = "willpower", max = 80, val = 5}}}

Species{
	name = "suicide bomber",
	base = "aer",
	categories = {"enemy"},
	difficulty = 0.5,
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather top", "leather pants", ["crimson grenade"] = 15},
	loot_categories = {"armor", "potion", "weapon"},
	loot_count = {2, 4},
	skills = {
		{name = "dexterity", max = 80, val = 5},
		{name = "health", max = 100, val = 100},
		{name = "intelligence", max = 100, val = 40},
		{name = "perception", max = 60, val = 40},
		{name = "strength", max = 40, val = 5},
		{name = "willpower", max = 80, val = 5}}}

require(Mod.path .. "effects")
require(Mod.path .. "items")
require(Mod.path .. "obstacles")
require(Mod.path .. "quests")
require(Mod.path .. "regions")
require(Mod.path .. "dialogs/bridgechest")
require(Mod.path .. "dialogs/erinyes")
require(Mod.path .. "dialogs/foreman")
require(Mod.path .. "dialogs/noemi")
require(Mod.path .. "dialogs/sanctuary")
require(Mod.path .. "patterns/brigandhq1")
require(Mod.path .. "patterns/chasm1")
require(Mod.path .. "patterns/erinyeslair1")
require(Mod.path .. "patterns/erinyesportal1")
require(Mod.path .. "patterns/midguard1")
require(Mod.path .. "patterns/noemi1")
require(Mod.path .. "patterns/ruins1")
require(Mod.path .. "patterns/ruins2")
require(Mod.path .. "patterns/ruins3")
require(Mod.path .. "patterns/sanctuary1")
