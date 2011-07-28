Dialogspec{name = "smallbuildings",
	{"branch", cond_dead = true,
		{"loot"}},
	{"choice","Build a small shop",
		{"spawn pattern", "castleshop", position_relative=Vector(0,2,0)}},
	{"choice","Build a garden",
		{"spawn pattern", "castlegarden", position_relative=Vector(0,1,0)}},
	{"choice","Build an apothecary",
		{"spawn pattern", "castleapothecary", position_relative=Vector(0,2,0)}},
	{"choice","Build a guard barracks",
		{"spawn pattern", "castlebarracks", position_relative=Vector(0,2,0)}},
	{"func", function(q)
						q.object:damaged{amount = 200, type = "physical"}
					end},
	{"exit"},
	{"choice", "Goodbye.",
		{"exit"}},
	{"loop"}}
