Program:add_path(Mod.path)

Quest{name = "Agathe's House"}
Quest{name = "Maurice's House"}
Quest{name = "The Hoarder"}
Faction{name = "silverspring"}
Faction{name = "the hoarder"}

Species{
	name = "Agathe",
	base = "aer",
	dialog = "agathe",
	marker = "agathe",
	ai_enable_combat = false,
	ai_enable_wander = false,
	factions = {"silverspring"},
	hair_style = {"hair1",0,255,192},
	inventory_items = {"dress"}}

Species{
	name = "Martin",
	base = "aermale",
	dialog = "martin",
	marker = "martin",
	dead = true,
	eye_style = {"default",0,0,0},
	hair_style = {"aermale1hair1",128,128,128},
	inventory_items = {"pants"}}

Species{
	name = "Maurice",
	base = "aermale",
	dialog = "maurice",
	marker = "maurice",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = {"default",64,192,64},
	factions = {"silverspring"},
	hair_style = {"bald",0,0,0},
	inventory_items = {"shirt", "pants"}}

Species{
	name = "The Hoarder",
	base = "aermale",
	dialog = "the hoarder",
	marker = "the hoarder",
	eye_style = {"default",255,32,32},
	factions = {"the hoarder"},
	hair_style = {"aermale1hair1",200,20,180},
	inventory_items = {"shirt", "pants", "leather boots"}}

require(Mod.path .. "silverspring1")
require(Mod.path .. "agathe")
require(Mod.path .. "martin")
require(Mod.path .. "maurice")
require(Mod.path .. "thehoarder")
