Actorspec{
	name = "Guy",
	base = "aer",
	ai_type = "camper",
	dialog = "guy",
	marker = "guy",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["dress"] = 1},
	preset = "Male 2",
	personality = "tough"}

Questspec{
	name = "Guy's Request"}

Dialogspec{name = "guy", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"say", "Guy", "Greetings, adventurer!"},
	{"branch",
		{"choice", "What should I do?",
			{"say", "Guy", "Enter that portal if you plan to be of any use."},
			{"notification", "Activate portals in the overworld to travel to dungeons."},
			{"say", "Guy", "You can also show off your mucles here, above the ground, like I do."},
			{"say", "Guy", "Or at least you can try, once you have some muscle."}},
		{"choice", "Do you have any work?",
			{"random quest", "Guy's Request", difficulty = "easy"}},
		{"choice", "Goodbye",
			{"exit"}},
		{"loop"}}}}

Dialogspec{name = "guy death", commands = {
	{"quest", "Guy's Request", status = "completed", marker = "guy", text = "Guy is dead."}}}
