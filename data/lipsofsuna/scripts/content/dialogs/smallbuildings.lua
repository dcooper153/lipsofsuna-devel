Dialogspec{name = "smallbuildings",
	{"branch", cond_dead = true,
		{"loot"}},
	{"choice","Build a north-south wall",
		{"spawn pattern", "wallns", position_relative=Vector(5,4,5)},{"exit"}},
	{"choice", "Goodbye.",
		{"exit"}},
	{"loop"}}
