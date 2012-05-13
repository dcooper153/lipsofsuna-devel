Obstaclespec{
	name = "portal of illusion",
	model = "portal1",
	categories = {["portal"] = true, ["special"] = true},
	dialog = "portal of illusion",
	marker = "portal of illusion",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "portal of illusion",
	commands = {
		{"teleport", marker = "chara's portal"},
		{"effect player", "portal1"}
	}}
