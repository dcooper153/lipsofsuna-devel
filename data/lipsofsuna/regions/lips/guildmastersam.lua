Dialogspec{name= "guildmastersam", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch", cond_not= "mining guild member",
		{"say","Guildmaster Sam","You there, want a once in a lifetime opportunity?"},
			{"choice","Yeah!",
				{"say","Guildmaster Sam","Good choice, knew you looked like a bright 'un"},
				{"say","Guildmaster Sam","You know, since this expedition started, the empires upground have been leaking cash left right and centre"},
				{"branch",
					{"choice","Hmm, interesting, go on..",
						{"say","Guildmaster Sam","This is not good for adventurers like yourself! If funds run out, there will be no more supplies comming down here, and I don't know about you, but I don't want to make the weeks long trek back upground on an empty stomach."},
						{"say","Guildmaster Sam","So, the collective empires wisely decided they've got to make this endevour pay for itself, and established a couple of guilds to seek out opportunities for profiting from this little... Situation"},
						{"say","Guildmaster Sam","I've always had a love for different kinds of shiny stones, and down here, as I'm sure you've seen, there are lots."},
						{"say","Guildmaster Sam","Enough to pay for an indefinite stay, methinks.."},
						{"say","Guildmaster Sam","Quite a few people agree with me in this, so we've created the Mining Guild to oversee the extraction of these minerals. We've got a number of members, hacking away in the caves as we speak."}},
					{"choice","Cut to the chase already..",{"break"}}},
				{"say","Guildmaster Sam","So what do you say, want to be part of the Mining Guild? We offer free mattocks for anyone who joins, quite the useful tool"},
					{"choice","Sounds like a good idea",
						{"say","Guildmaster Sam","Excellent! I'll just do the paperwork, come back to me in a bit and I'll give you the pick."},
						{"flag","mining guild member"},
						{"quest", "Mining Guild", status = "active", marker = "celine", text = "Joined the mining guild for fun and profit"},
						{"exit"}
					},
					{"choice","No, this place is sacred! How can you rape the sanctuary of the world tree, you beast!",
						{"exit"}
					}
			}
	},
	{"branch", cond="mining guild member",
		{"say","Guildmaster Sam","Ah, welcome back, guildmember"},
		{"choice","Where's that mattock you promised me?",
			{"say","Guildmaster Sam","Oh, heh, I have one in a pile here..."},
			{"say","Guildmaster Sam","Here you go"},
			{"give player item","iron mattock"}},
		{"choice","Got any jobs for me?",
			{"branch",cond_not="bridge completed",{"branch",cond = "bridge builder requested",
				{"say","Guildmaster Sam","Hey, you haven't finished with the bridge yet"},{"exit"}}},
			{"branch",cond_not = "bridge builder requested",
				{"say","Guildmaster Sam","Good you asked! One of our major mining routes has been crippled, something blew up the bridge we use to move ores over."},
				{"say","Guildmaster Sam","I need someone to go down there who is able to fix it, the darn foreman doesn't know anything about construction"},
				{"say","Guildmaster Sam","I would ask you if you're up for it, but you asked for a job, and there really isn't any reason for you to not do this one. Now run along"},
				{"flag","bridge builder requested"},
				{"quest", "Bridge Building", status = "active", marker = "foreman", text = "The Mining Guild's Guildmaster asked you to repair a brige along a mining route. Find the foreman and report to him for further instructions"},{"exit"}},
			{"branch",cond_not = "brigand quest known",
				{"say","Guildmaster Sam","Now that you've proved yourself, we have damn brigands harrasing our miners, and it's costing us dearly"},
				{"say","Guildmaster Sam","It will help us greatly if you could.. Remove them somehow."},
				{"flag","brigand quest known"},
					{"choice","Brigands! I'll take care of it",
						{"quest", "Brigands", status = "active", marker = "brigandking", text = "Guildmaster Sam asked me to eliminate the brigands harrasing the mining guild"},
						{"flag","doing brigand quest"},
						{"say","Guildmaster Sam","Great, eliminating the brigands will make our quarter report look a lot more positive"},
						{"say","Guildmaster Sam","All you have to do is infiltrate their camp, and kill their king. Without leadership, they'll be much less of a problem"}
					},
					{"choice","I'm a miner, not a mercenary! They'll tear me to shreds",
						{"say","Guildmaster Sam","Hmm, I was under the impression that most of the people who venture these depths were.. Anyhow, come back if you change your mind"},
						{"exit"}
					}
			},
			{"branch",cond_not="doing brigand quest",
				{"say","Guildmaster Sam","Grown a pair since we last talked?"},
				{"choice","Err, I guess, those brigands aren't so tough",
					{"quest", "Brigands", status = "active", marker = "brigandking", text = "Guildmaster Sam asked me to eliminate the brigands harrasing the mining guild"},
					{"flag","doing brigand quest"},
					{"say","Guildmaster Sam","Great, eliminating the brigands will make our quarter report look a lot more positive"},
					{"say","Guildmaster Sam","All you have to do is infiltrate their camp, and kill their king. Without leadership, they'll be much less of a problem"}
				},
				{"choice","I guess not.."}
			}
		},
		{"choice","Goodbye",{"exit"}},
		{"loop"}
	}
}}
