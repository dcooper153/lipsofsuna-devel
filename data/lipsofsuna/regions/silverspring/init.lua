Program:add_path(Mod.path)

Questspec{
	name = "Agathe's House"}

Questspec{
	name = "Maurice's House"}

Questspec{
	name = "The Hoarder"}

Factionspec{
	name = "silverspring"}

Factionspec{
	name = "the hoarder"}

Actorspec{
	name = "Agathe",
	base = "aer",
	ai_type = "camper",
	dialog = "agathe",
	marker = "agathe",
	factions = {["silverspring"] = true},
	hair_color = {0,255,192},
	hair_style = "hair1",
	important = true,
	inventory_items = {["dress"] = 1}}

Actorspec{
	name = "Martin",
	base = "aermale",
	dialog = "martin",
	marker = "martin",
	dead = true,
	eye_color = {0,0,0},
	eye_style = "default",
	hair_color = {128,128,128},
	hair_style = "aermale1hair1",
	important = true,
	inventory_items = {["pants"] = 1}}

Actorspec{
	name = "Maurice",
	base = "aermale",
	ai_type = "camper",
	dialog = "maurice",
	marker = "maurice",
	eye_color = {64,192,64},
	eye_style = "default",
	factions = {["silverspring"] = true},
	hair_color = {0,0,0},
	hair_style = "bald",
	important = true,
	inventory_items = {["shirt"] = 1, ["pants"] = 1}}

Actorspec{
	name = "The Hoarder",
	base = "aermale",
	ai_type = "camper",
	dialog = "the hoarder",
	marker = "the hoarder",
	eye_color = {255,32,32},
	eye_style = "default",
	factions = {["the hoarder"] = true},
	hair_color = {200,20,180},
	hair_style = "aermale1hair1",
	important = true,
	inventory_items = {["shirt"] = 1, ["pants"] = 1, ["leather boots"] = 1}}

require(Mod.path .. "silverspring1")
require(Mod.path .. "agathe")
require(Mod.path .. "martin")
require(Mod.path .. "maurice")
require(Mod.path .. "thehoarder")
