Dialogspec{name = "smallbuildings",
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch",
	{"choice","Build a small wall in this direction:",
		{"choice","Cancel",{"break"}},
		{"choice","East to West",	{"spawn pattern", "swallns", position_relative=Vector(0,2,0),rotation=1},{"var","built"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "swallns", position_relative=Vector(0,2,0)},{"var","built"},{"break"}},
		{"choice","West to East",	{"spawn pattern", "swallns", position_relative=Vector(0,2,0),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "swallns", position_relative=Vector(0,2,0),rotation=2},{"var","built"},{"break"}}},
	{"choice","Build a small shop facing this direction:",
		{"choice","Cancel",{"break"}},
		{"choice","East to West",	{"spawn pattern", "castleshop", position_relative=Vector(0,2,0),rotation=1},{"var","built"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "castleshop", position_relative=Vector(0,2,0)},{"var","built"},{"break"}},
		{"choice","West to East",	{"spawn pattern", "castleshop", position_relative=Vector(0,2,0),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "castleshop", position_relative=Vector(0,2,0),rotation=2},{"var","built"},{"break"}}},
	{"choice","Build a garden",
		{"spawn pattern", "castlegarden", position_relative=Vector(1,1,1)}},
	{"choice","Build an apothecary facing this direction",
		{"choice","Cancel",{"break"}},
		{"choice","East to West",	{"spawn pattern", "castleapothecary", position_relative=Vector(0,2,0),rotation=1},{"var","built"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "castleapothecary", position_relative=Vector(0,2,0)},{"var","built"},{"break"}},
		{"choice","West to East",	{"spawn pattern", "castleapothecary", position_relative=Vector(0,2,0),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "castleapothecary", position_relative=Vector(0,2,0),rotation=2},{"var","built"},{"break"}}},
	{"choice","Build a guard barracks facing this direction",
		{"choice","Cancel",{"break"}},
		{"choice","East to West",	{"spawn pattern", "castlebarracks", position_relative=Vector(0,2,0),rotation=1},{"var","built"},{"break"}},
		{"choice","North to South",	{"spawn pattern", "castlebarracks", position_relative=Vector(0,2,0)},{"var","built"},{"break"}},
		{"choice","West to East",	{"spawn pattern", "castlebarracks", position_relative=Vector(0,2,0),rotation=3},{"var","built"},{"break"}},
		{"choice","South to North",	{"spawn pattern", "castlebarracks", position_relative=Vector(0,2,0),rotation=2},{"var","built"},{"break"}}},
	{"choice", "Cancel",
		{"exit"}}},
	{"branch", check="var:built",
	{"func", function(q)
						q.object:damaged{amount = 200, type = "physical"}
					end}},
	{"loop"}}
