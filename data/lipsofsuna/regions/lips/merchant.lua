Species{
	name = "Merchant",
	base = "aer",
	dialog = "merchant",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"},
	ai_type = "anchored",
	personality = "noble"}

Dialogspec{name = "merchant",
	{"branch", cond_dead = true,
		{"loot"}},
	{"trade"}}
