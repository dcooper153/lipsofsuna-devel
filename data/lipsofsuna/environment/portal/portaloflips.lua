Obstaclespec{
	name = "portal of lips",
	model = "portal1",
	categories = {["portal"] = true, ["special"] = true},
	dialog = "portal of lips",
	marker = "portal of lips",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "portal of lips",
	commands = {
		{"branch", cond = "portal of lips activated",
			{"branch", cond = "portal of midguard activated",
				{"teleport", marker = "portal of midguard"},
				{"effect player", "portal1"},
				{"exit"}
			},
			{"info", "The other end of the portal is closed."},
			{"exit"}
		},
		{"quest", "Portal of Lips", status = "completed", marker = "portal of lips", text = "The portal of Lips has been activated."},
		{"flag", "portal of lips activated"},
		{"info", "You have activated the portal of Lips."}
	}}
