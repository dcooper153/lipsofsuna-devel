Obstaclespec{
	name = "chara's portal",
	model = "portal1",
	categories = {["portal"] = true, ["special"] = true},
	dialog = "chara's portal",
	marker = "chara's portal",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "chara's portal",
	commands = {
		{"branch", cond = "illusion opened", 
			{"teleport", marker = "portal of illusion"},
			{"effect player", "portal1"},
			{"exit"}
		},
		{"branch", cond_not = "illusion opened",
			{"info", "The portal doesn't seem to be active."}}
		}}
