Dialogspec{name = "chara's portal", unique = true,
	{"branch", cond = "illusion opened", 
		{"teleport", marker = "portal of illusion"},
		{"effect player", "portal1"},
		{"exit"}},
	{"branch", cond_not = "illusion opened",
		{"info", "The portal doesn't seem to be active."}}}
