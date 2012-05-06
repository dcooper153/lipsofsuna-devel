Actorspec{
	name = "Lips guard archer",
	ai_type = "camper",
	base = "aer",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["iron greaves"] = 1, ["iron breastplate"] = 1, ["iron gauntlets"] = 1, ["crimson crossbow"] = 1, ["arrow"] = 100},
	personality = "lawful",
	skills = {["Health lv2"] = true}}

Actorspec{
	name = "Lips guard knight",
	base = "aer",
	ai_type = "camper",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["iron greaves"] = 1, ["iron breastplate"] = 1, ["iron gauntlets"] = 1, ["crimson sword"] = 1, ["round shield"] = 1},
	personality = "lawful",
	skills = {["Health lv4"] = true}}

Actorspec{
	name = "Lips guard mage",
	base = "kraken",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["lizard dress"] = 1, ["lizard gloves"] = 1, ["milky powder"] = 100},
	skills = {["Health lv1"] = true, ["Willpower lv4"] = true}}

Dialogspec{name = "lips guard", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"say", "Guard", "Greetings!"},
	{"branch",
		{"choice", "What places are there in these dungeons?",
			{"say", "Guard", "Here, they're marked in your map."},
			{"branch", cond_not = "portal of lips activated",
				{"quest", "Portal of Lips", status = "active", marker = "portal of lips", text = "We have learned the location of the portal of Lips."}},
			{"branch", cond_not = "sanctuary activated",
				{"quest", "Sanctuary", status = "active", marker = "sanctuary", text = "We have learned the location of the Sanctuary."}},
			{"branch", cond_not = "silverspring known",
				{"quest", "The Hoarder", status = "active", marker = "the hoarder", text = "We have learned the location of Silverspring."}},
		},
		{"choice", "Goodbye.",
			{"exit"}},
		{"loop"}}}}
