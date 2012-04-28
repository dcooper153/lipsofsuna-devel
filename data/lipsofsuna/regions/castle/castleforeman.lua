Dialogspec{name = "castleforeman", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"say","Castle Foreman","I presume you're the new lord around these parts"},
	{"choice", "Goodbye.",
		{"exit"}},
	{"loop"}}}
