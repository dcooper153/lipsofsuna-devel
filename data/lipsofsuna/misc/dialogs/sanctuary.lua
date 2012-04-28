Dialogspec{name = "sanctuary", commands = {
	{"branch", cond_not = "sanctuary activated",
		{"quest", "Sanctuary", status = "completed", marker = "sanctuary", text = "The Sanctuary has been activated."},
		{"flag", "sanctuary activated"},
		{"info", "You have activated the Sanctuary."},
		{"exit"}},
	{"info", "The Sanctuary is already active."}}}
