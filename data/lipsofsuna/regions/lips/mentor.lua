Actorspec{
	name = "Mentor",
	base = "aermale",
	ai_type = "campter",
	dialog = "mentor",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["shirt"] = 1, ["pants"] = 1, ["leather boots"] = 1},
	marker = "mentor"}

Dialogspec{name = "mentor", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"say", "Mentor", "Hello there! Welcome to Lips! The world is still in developmental turmoil, so excuse the mess. Oh, also I was said to tell you this when you arrive by a cloaked figure, it seemed to be a man, but he didn’t say his name and shortly left afterwards. It doesn’t make much sense to me, so maybe you’ll have better luck! He said “Jump with Space, Climb with C, Swap to First-Person mode with Y, press Esc for the menu (and look at the further help there), I hope you’ve worked out how to move by now"},
	{"branch",
		{"choice", "Where am I?",
			{"say", "Mentor", "In Lips, like I said! I hope you enjoy your stay, for out there in the caverns is danger, dangerous danger even!"}},
		{"choice","Who are you?",
			{"say", "Mentor","I’m Sicrates, although most people call me Mentor for some reason. You should see me around town, handing out advice to those who need it, and making them pay through the teeth! *Mentor laughs heartily* Not you though, you’ll be spared for now!*Mentor laughs heartily once more*"}},
		{"choice", "Goodbye",
			{"exit"}},
		{"loop"}}
	}}
