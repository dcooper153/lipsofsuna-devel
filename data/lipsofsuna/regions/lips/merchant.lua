Actorspec{
	name = "Merchant",
	base = "aer",
	ai_type = "anchored",
	dialog = "merchant",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["dress"] = 1},
	personality = "noble"}

Dialogspec{name = "merchant", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"trade"}}}
