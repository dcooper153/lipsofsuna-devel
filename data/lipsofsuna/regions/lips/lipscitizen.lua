Actorspec{
	name = "Lips citizen",
	base = "aer",
	ai_type = "camper",
	dialog = "lips citizen",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["leather top"] = 1, ["leather pants"] = 1, ["leather leggings"] = 1},
	personality = "feeble"}

Dialogspec{name = "lips citizen", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"say", "Citizen", "Greetings!"},
	{"branch",
		{"choice", "Tell me about the town.",
			{"say", "Citizen", "The deserted town was here already when the first heroes arrived."},
			{"say", "Citizen", "I think the town appeared out of nowhere when the World Tree fell."}},
		{"choice", "Anything intreresting going on?",
			{"branch", cond_not = "peculiar pet completed",
				{"say", "Citizen", "Celine has been acting strange lately."},
				{"say", "Citizen", "She lives in the north-west corner of the town."}},
			{"branch", cond = "peculiar pet completed",
				{"say", "Citizen", "Nothing apart from the end of the world."}}},
		{"choice", "Goodbye.",
			{"exit"}},
		{"loop"}}}}
