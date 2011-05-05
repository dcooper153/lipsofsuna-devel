Dialogspec{name="mentor", unique = true,
	{"branch", cond_dead = true,
		{"loot"}},
	{"say", "Mentor", "Hello there! Welcome to Lips! The world is still in developmental turmoil, so excuse the mess. Jump with C, Swap to First-Person mode with Y, press Esc for the menu (and look at the further help there)"},
	{"branch",
		{"choice", "Where am I?",
			{"say", "Mentor", "In Lips, like I said!"}},
		{"choice", "Goodbye",
			{"exit"}},
		{"loop"}}
	}