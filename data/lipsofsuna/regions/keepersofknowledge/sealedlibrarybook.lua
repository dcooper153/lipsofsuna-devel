Dialogspec{name = "sealedlibrarybook", commands = {
	{"branch", cond_not = "sealed library known",
		{"quest", "Keepers of Knowledge", status = "active", marker = "sealed library", text = "A sealed library full of preserved knowledge lies somewhere in the dungeons."}},
		{"flag", "sealed library known"}}}
