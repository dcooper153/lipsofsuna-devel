Species{
	name = "Zyra the Mage",
	base = "aer",
	dialog = "zyra",
	marker = "zyra",
	ai_type = "anchored",
	eye_style = "random",
	hair_style = "random",
	personality = "noble",
	inventory_items = {"dress"}}

Dialogspec{name = "zyra",
	{"default death check"},
	{"branch", check = {{"!flag", "zyra complained"}},
		{"flag", "zyra complained"},
		{"say", "Zyra the Mage", "I can't believe they're making me do this..."},
		{"say", "Zyra the Mage", "I come here to study the largest discovery in all history, and I'm assigned as a trainer for idiots!"},
		{"say", "Zyra the Mage", "Oh, didn't see you there. I guess you're here to learn how to cast spells."}
	},
	{"branch",
		{"choice", "How can I cast spells?",
			{"say", "Zyra the Mage", "You need reagents and willpower to cast a spell."},
			{"notification", "The two bars at the bottom right corner are your health and willpower."},
			{"say", "Zyra the Mage", "Willpower you hopefully have of your own."},
			{"say", "Zyra the Mage", "Reagents need to be collected. I can give you some so that you can practice."},
			{"give player item", "milky powder", count = 20},
			{"notification", "Switch to the spells quickbar with TAB and cast spells with number keys."},
			{"say", "Zyra the Mage", "Try to cast a fireball with those reagents."}
		},
		{"choice", "How can I cast more advanced spells.",
			{"notification", "Enter the spell editor with F. Create a spell by selecting its slot, type and effects."},
			{"say", "Zyra the Mage", "It's easy. Anyone can come up with their own spells by combining multiple effects."},
			{"say", "Zyra the Mage", "Remember to pay attention to the reagent and willpower requirements or you might not be able to cast the spell."},
			{"notification", "See Help->Spells in the main menu for more information on spells."}
		},
		{"choice", "Goodbye.",
			{"exit"}},
		{"loop"}
	}}   
