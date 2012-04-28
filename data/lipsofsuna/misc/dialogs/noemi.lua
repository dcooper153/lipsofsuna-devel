Dialogspec{name = "noemi", commands = {
	{"branch", cond_not = "noemi found",
		{"quest", "Elise's Sorrow", status = "active", marker = "elise", text = "Noemi is dead. We should return to Elise to tell about it."},
		{"flag", "noemi found"}},
	{"info", "She's dead."}}}
