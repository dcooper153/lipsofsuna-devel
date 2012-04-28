Actorspec{
	name = "Fighting Instructor Fa",
	base = "wyrm",
	dialog = "fa",
	marker = "fa",
	ai_type = "anchored",
	eye_style = "random",
	hair_style = "random",
	personality = "savage",
	inventory_items = {["iron breastplate"] = 1, ["iron greaves"] = 1, ["iron gauntlets"] = 1, ["wooden staff"] = 1}}

Dialogspec{name = "fa", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch",cond_not="learning fighting",
		{"say", "Fighting Instructor Fa","Another one? It must be getting really bad upworld.."},
		{"say", "Fighting Instructor Fa","Lets get on with it. Take this stick, equip it (hit I to go to inventory,right click the staff) or trigger its quick slot"},
		{"give player item","wooden staff"},
		{"say", "Fighting Instructor Fa","Now go and bash the brains out of Dummy. Pretty easy, swap to first person mode using Y, and click to attack it. It should have a token in it's inventory, bring it to me and you'll have completed the fighting quest"},
		{"flag","learning fighting"},
		{"quest", "An Introduction to Ultraviolence", status = "active", marker = "dummy", text = "Fa told me to destroy an frighteningly alive looking Dummy. Remember to swap to first person (Y)"}
	},
	{"branch",cond_not="done fighting",
		{"say","Fighting Instructor Fa","Did you get that token?"},
		{"choice","Yes",
			{"require player item", "combat token",
				{"branch",
					{"say","Fighting Instructor Fa","Nothing like a bit of the old Ultra-Violence, ey? Now, go to Spud and show him this token"},
					{"quest", "An Introduction to Ultraviolence", status = "completed", marker = "spud", text = "I have learnt how to fight, and recieved the neccessary token"},
					{"unlock reward"},
					{"flag","done fighting"}},	
				{"say","Fighting Instructor Fa","No, you bloody well haven't. What are you some kind of pansy pacifist? Go and get my token!"}
			}
		},
		{"choice","Not yet",
			{"say","Fighting Instructor Fa","You better watch out, Spud has been known to gobble up tardy adventurers..."}
		},
		{"choice","Err, what should I do again?",
			{"say","Fighting Instructor Fa","Kill Dummy with your staff. Or your fists, but you better learn how to equip weapons!"}
		}
	},
	{"branch",cond="done fighting",
		{"say", "Fighting Instructor Fa", "I've taught you what I know. There's more to fighting yet, like ranged weapons, grenades, directional attacks, but I'll leave that for you to discover further. Now, go on to the other trainers, or talk to Spud"},
		{"exit"}
	},
	{"loop"}}}
