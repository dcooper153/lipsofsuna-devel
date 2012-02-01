Dialogspec{name = "the hoarder", unique = true,
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch",
		{"choice", "Who are you?",
			{"say", "The Hoarder", "Serge Oakland is who I once were"},
			{"say", "The Hoarder", "These days the ungrateful townspeople call me The Hoarder."},
			{"say", "The Hoarder", "As if they weren't enough trouble already."}},
		{"choice", "Let's talk about the villagers.",
			{"choice", "Why do people live outdoors in this village?",
				{"say", "The Hoarder", "They're lucky that they can stay in the village at all."},
				{"say", "The Hoarder", "They traded the land for their debt and start burning trees in a bonnfire without permission."},
				{"say", "The Hoarder", "I wish someone could stop them from destroying my property."},
				{"branch", cond_not = "silverspring hoarder problem known",
					{"quest", "The Hoarder", status = "active", marker = "the hoarder", text = "The Hoarder of Silvespring has told us about a problem with the townspeople. They sold the land to him but now keep burning the trees in a bonnfire."},
					{"flag", "silverspring known"},
					{"flag", "silverspring poverty known"},
					{"flag", "silverspring hoarder problem known"}}},
			{"choice", "Why did they have so much debt that they had to sell everyting?", cond = "silverspring poverty known",
				{"say", "The Hoarder", "The reason varies from case to case."},
				{"say", "The Hoarder", "Inability to plan for the future..."},
				{"say", "The Hoarder", "Failure to take care of oneself..."},
				{"say", "The Hoarder", "To name some of them."}},
			{"choice", "Why was Maurice in debt?", cond = "maurice known",
				{"say", "The Hoarder", "TODO"}},
			{"choice", "Why was Agathe in debt?", cond = "agathe known",
				{"say", "The Hoarder", "TODO"}},
			{"choice", "Let's talk about something else.",
				{"break", 2}},
			{"loop"}},
		{"choice", "Goodbye.",
			{"exit"}},
		{"loop"}}}

Dialogspec{name = "the hoarder death",
	{"quest", "The Hoarder", status = "completed", marker = "the hoarder", text = "The Hoarder has been killed."},
	{"flag", "silverspring known"},
	{"flag", "the hoarder dead"}}