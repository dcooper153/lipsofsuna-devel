Species{
	name = "Craftress Xsphin",
	base = "kraken",
	dialog = "xsphin",
	marker = "xsphin",
	ai_type = "anchored",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"}}

Dialogspec{name = "xsphin",
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch",cond_not="learning crafting",
		{"say", "Craftress Xsphin","Sssswelcome, I presssume you're here to learn the art of crafting. Ssss"},
		{"say", "Craftress Xsphin","It'ssss rather.. easssssss... No, I can't keep this up, this stereotype is insulting to our kind."},
		{"say", "Craftress Xsphin","Gah, I have to teach you anyhow. Here, take this wood, right click on the Workbench (anvil icon) in your inventory (press I), or activate its quickslot (3, usually), or walk over to the one behind me and hit E."},
		{"give player item","log"},{"give player item","log"},{"give player item","log"},
		{"say", "Craftress Xsphin","Drag the wood into one of the top slots in the workbench, and find the Wood Dagger. Click it, and you'll have crafted yourself a dagger."},
		{"say", "Craftress Xsphin","Bring it to me, and I'll give you the token you need to get out of here."},
		{"flag","learning crafting"},
		{"quest", "An Introduction to Crafting", status = "active", marker = "xsphin", text = "Xsphin told me to craft a wooden dagger using the logs she gave me. I have to open up the workbench, insert the wood, and look for a wooden dagger."}
	},
	{"branch",cond_not="done crafting",
		{"say","Craftress Xsphin","How are things coming along, do you have a dagger yet?"},
		{"choice","Yes",
			{"require player item", "wooden dagger",
				{"branch",
					{"say","Craftress Xsphin","Ah, good, good. And guessssss -dammit- what, it's your token as well. Now, go report to Zyra, or Fa, or Spud if you've completed your training"},
					{"quest", "An Introduction to Crafting", status = "completed", marker = "spud", text = "I have learnt how to craft, and recieved the neccessary token"},
					{"unlock reward"},
					{"flag","done crafting"}},	
				{"say","Craftress Xsphin","No, you haven't made a dagger. If you've wasted your wood, we have a problem"}}},
		{"choice","Not yet",
			{"say","Craftress Xsphin","You better watch out, Spud has been known to gobble up tardy adventurers..."}},
		{"choice","Err, what should I do again?",
			{"say","Craftress Xsphin","Denser than a lump of granite.. Right click on the Workbench (anvil icon) in your inventory (press I), drag wood, look for wooden dagger, click on it. Not that hard."}
		}
	},
	{"branch",cond="done crafting",
		{"say", "Craftress Xsphin", "I've taught you what I know. You'll have to discover the art of crafting further by yourself. Now, go on to the other trainers, or talk to Spud"},
		{"exit"}
	},
	{"loop"}}
