Dialogspec{name = "lips guard",
	{"branch", cond_dead = true,
		{"loot"}},
	{"say", "Guard", "Greetings!"},
	{"branch",
		{"choice", "What places are there in these dungeons?",
			{"say", "Guard", "Here, they're marked in your map."},
			{"branch", cond_not = "portal of lips activated",
				{"quest", "Portal of Lips", status = "active", marker = "portal of lips", text = "We have learned the location of the portal of Lips."}},
			{"branch", cond_not = "sanctuary activated",
				{"quest", "Sanctuary", status = "active", marker = "sanctuary", text = "We have learned the location of the Sanctuary."}},
			{"branch", cond_not = "silverspring known",
				{"quest", "The Hoarder", status = "active", marker = "the hoarder", text = "We have learned the location of Silverspring."}},
		},
		{"choice", "Goodbye.",
			{"exit"}},
		{"loop"}}}