Actorspec{
	name = "Guy",
	base = "aer",
	dialog = "guy",
	marker = "guy",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["dress"] = 1},
	preset = "Male 2",
	ai_type = "camper",
	personality = "tough"}

Questspec{
	name = "Guy's Request"}

Dialogspec{name = "guy", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"say", "Guy", "Greetings, adventurer!"},
	{"branch",
		{"choice", "What should I do?",
			{"say", "Guy", "You enter that portal if you plant to be of any use."},
			{"notification", "Activate portals in the overworld to travel to dungeons."},
			{"say", "Guy", "You can also slack off here above the ground like I do."},
			{"say", "Guy", "I does not matter either way."}},
		{"choice", "Do you have any work?",
			{"random quest", "Guy's Request", difficulty = "easy"}},
		{"choice", "Goodbye",
			{"exit"}},
		{"loop"}}}}

Dialogspec{name = "guy death", commands = {
	{"quest", "Guy's Request", status = "completed", marker = "guy", text = "Guy is dead."}}}
