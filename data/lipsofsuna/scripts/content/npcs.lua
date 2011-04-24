require "content/species"

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
	name = "Chara",
	base = "seirei",
	dialog = "chara",
	marker = "chara"}

Species{
	name = "Celine",
	base = "aer",
	dialog = "celine",
	marker = "celine",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair1",0,255,0},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"peculiar pet"}}

Species{
	name = "Elise",
	base = "aer",
	animations = {
		["idle"] = {animation = "tired", channel = 1, permanent = true}},
	dialog = "elise",
	marker = "elise",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"neutral"}}

Species{
	name = "Erinyes",
	base = "seirei",
	dialog = "erinyes",
	marker = "erinyes"}

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
	name = "Noemi",
	base = "aer",
	dialog = "noemi",
	marker = "noemi",
	dead = true,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"good"}}

Species{
	name = "Puppy",
	base = "bloodworm",
	categories = {},
	dialog = "puppy",
	marker = "puppy",
	ai_enable_combat = false,
	ai_enable_wander = false,
	factions = {"peculiar pet"}}

Species{
	name = "Roland",
	base = "aermale",
	dialog = "roland",
	marker = "roland",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = {"default",255,0,0},
	factions = {"neutral"},
	hair_style = {"aermale1hair1",128,128,128},
	inventory_items = {"shirt", "pants", "leather boots"}}

Species{
	name = "The Hoarder",
	base = "aermale",
	dialog = "the hoarder",
	marker = "the hoarder",
	eye_style = {"default",255,32,32},
	factions = {"the hoarder"},
	hair_style = {"aermale1hair1",200,20,180},
	inventory_items = {"shirt", "pants", "leather boots"}}
