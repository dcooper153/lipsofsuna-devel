Actorspec{
	name = "Merchant",
	base = "aer",
	dialog = "merchant",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["dress"] = 1},
	ai_type = "anchored",
	personality = "noble"}

Dialogspec{name = "merchant", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"trade"}}}
