Actorspec{
	name = "Celine",
	base = "aer",
	ai_type = "camper",
	dialog = "celine",
	factions = {["peculiar pet"] = true},
	hair_color = {0,255,0},
	hair_style = "hair1",
	important = true,
	inventory_items = {["iron hatchet"] = 1, ["dress"] = 1},
	marker = "celine",
	personality = "feeble",
	preset = "Female 2"}

Dialogspec{name = "celine", commands = {
	{"default death check"},
	{"branch", check = {{"flag", "imaginary friend completed"}},
		{"choice", "Have you been feeling alright lately?",
			{"say", "Celine", "A cat scratched my toe the other day."},
			{"say", "Celine", "I'm worrying that I might die."}
		},
		{"choice", "Goodbye.",
			{"exit"}
		},
		{"loop"}
	},
	{"say", "Celine", "Isn't she adorable?"},
	{"branch",
		{"choice", "What are you talking about?",
			{"say", "Celine", "This cute, furry, little lamb of course."},
			{"branch", check = {{"!flag", "hallucinations known"}},
				{"quest", "Imaginary Friend", status = "active", marker = "roland", text = "We have met Celine who appears to be hallucinating. Perhaps a doctor knows a way to cure her."},
				{"flag", "hallucinations known"}
			},
			{"choice", "I don't see a lamb there.",
				{"say", "Celine", "Oh, I get it, you got to be another blind guy."},
				{"say", "Celine", "There sure are lots of you here."},
				{"say", "Celine", "Everyone's about to step on her as if she was thin air."},
			},
			{"choice", "Err, sure, she's cute.",
				{"say", "Celine", "Isn't she? I found her when I was collecting mushrooms."},
				{"say", "Celine", "I lost my consciousness there for some reason but it wasn't anything serious."},
				{"say", "Celine", "When I woke up, she was licking and nibbing me."},
				{"say", "Celine", "She followed me all the way here. I think she likes me."},
				{"say", "Celine", "Ah, don't try to lick me there again, you silly!"}
			},
			{"choice", "Trade for a one-horned, ham-handed, split-dicked titmouse!",
				{"say", "Celine", "No way! I'm not giving her to you."},
				{"say", "Celine", "Look now, you're scaring her."},
				{"say", "Celine", "What kind of dork would trade pets like that anyway?"},
				{"choice", "Sounds like you're asking for a pet battle...",
					{"flag", "pet battle joke"},
					{"say", "Celine", "No, no, not at all. I was just kidding."},
					{"say", "Celine", "Ahahaha..."},
				},
				{"choice", "Whatever."}
			},
			{"choice", "I got to go...",
				{"exit"}
			}
		},
		{"choice", "I need to talk about a more important matter.", check = {{"flag", "hallucinations known"}}},
	},
	{"branch",
		{"choice", "Have you been feeling weird lately?",
			{"say", "Celine", "Not at all."},
			{"say", "Celine", "My head has been hurting badly lately."},
			{"say", "Celine", "My vision has been blurry occasionally."},
			{"say", "Celine", "I have been hearing voices as well."},
			{"say", "Celine", "Nothing to worry about."},
		},
		{"choice", "[Cure disease potion] You look thirsty, have a drink.", check = {{"flag", "black-spotted parasite known"}},
			{"remove player item", "cure disease potion",
				{"branch",
					{"say", "Celine", "Wow, my headache disappeared."},
					{"say", "Celine", "But where did the lamb go?"},
					{"choice", "You were hallucinating.",
						{"say", "Celine", "Perhaps it was because of that funny mushroom."},
						{"say", "Celine", "It must have been a really funny one to last this long."}
					},
					{"choice", "In your imaginary pet ball, obviously.", check = {{"flag", "pet battle joke"}},
						{"say", "Celine", "Ahahaha..."}
					},
					{"quest", "Imaginary Friend", status = "completed", marker = "celine", text = "Celine has been cured by our potion."},
					{"flag", "imaginary friend completed"},
					{"exit"}
				},
				{"branch",
					{"say", "Celine", "I see it clearly, you don't have anything there."},
					{"say", "Celine", "Nothing can fool my eyes."}
				},
			}
		},
		{"choice", "[Poison potion] You look thirsty, have a drink.", check = {{"flag", "black-spotted parasite known"}},
			{"remove player item", "poison potion",
				{"branch",
					{"func", [[q.object:say("Aaaahhhh!")
						q.object:die()
						q.object:damaged{amount = 200, type = "physical"}]]},
					{"spawn object", "bloodworm", position_relative = Vector(0,3,0)},
					{"quest", "Imaginary Friend", status = "completed", marker = "celine", text = "Celine is dead. Her brain parasite broke through her skull violently when escaping the poison of the potion Celine drank."},
					{"flag", "imaginary friend completed"},
					{"exit"}
				},
				{"branch",
					{"say", "Celine", "I see it clearly, you don't have anything there."},
					{"say", "Celine", "Nothing can fool my eyes."}
				}
			}
		},
		{"choice", "Goodbye.",
			{"exit"}
		},
		{"loop"}
	}
}}

Dialogspec{name = "celine death", commands = {
	{"branch", check = {{"!flag", "imaginary friend completed"}},
		{"quest", "Imaginary Friend", status = "completed", marker = "celine", text = "Celine has been killed."}
	},
	{"branch", check = {{"flag", "imaginary friend completed"}},
		{"quest", "Imaginary Friend", status = "completed", marker = "celine", text = "Celine has been cured, permanently."}
	},
	{"flag", "imaginary friend completed"}
}}
