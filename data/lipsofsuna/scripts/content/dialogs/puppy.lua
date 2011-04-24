Dialogspec{name = "puppy",
	{"branch", cond_dead = true,
		{"loot"}},
	{"random",
		{"info", "The foul beast extends its tongue."},
		{"info", "The foul beast stares you hungrily."},
		{"info", "The foul beast grins widely."},
		{"info", "The foul beast salivates blood."}},
	{"choice", "(Touch the monster.)",
		{"info", "The squirmy and stinky worm feels sticky and pulsing hot to touch."},
		{"branch", cond_not = "peculiar pet completed",
			{"branch", cond = "puppy known",
				{"branch", cond_not = "puppy touched",
					{"quest", "Peculiar Pet", status = "active", text = "Celine has called us crazy because we told her that her pet, Puppy, is a bloodworm. However, no matter how you look at it, the bloodworm really is a bloodworm. It looks like one, feels like one, and even stinks like one."},
					{"flag", "puppy touched"},
					{"flag", "hallucinations known"}}}}},
	{"choice", "(Leave the monster.)",
		{"exit"}}}

Dialogspec{name = "puppy death",
	{"branch", cond_not = "peculiar pet completed",
		{"quest", "Peculiar Pet", status = "active", text = "The bloodworm that Celine treated as her pet has been killed."},
		{"flag", "puppy dead"}}}
