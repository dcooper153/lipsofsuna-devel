Dialogspec{name = "librarysecurityenforcer", commands = {
	{"branch", cond_dead = true,
		{"loot"}},
	{"branch", cond = "sealed library owned",
		{"say", "Library security enforcer", "Your access to the library has been permitted, welcome."},
		{"exit"}},
	{"say", "Library security enforcer", "Halt, outsider."},
	{"branch",
		{"choice", "What's this place?",
			{"say", "Library security enforcer", "This is the third side branch of the great library."},
			{"flag", "sealed library known"},
			{"say", "Library security enforcer", "The library has been sealed and in emergency mode for..."},
			{"say", "Library security enforcer", "*Beep*"},
			{"say", "Library security enforcer", "...minus seven hundred twenty two years."},
			{"say", "Library security enforcer", "System error, severity level three, topic terminated."},
		},
		{"choice", "What are you?",
			{"say", "Library security enforcer", "I am a library security enforcer."},
			{"say", "Library security enforcer", "I have been deployed to protect knowledge from unauthorized access."},
		},
		{"choice", "May I access the library?", cond = "sealed library known",
			{"say", "Library security enforcer", "Access to the library is denied."},
			{"say", "Library security enforcer", "To gain access, deposit ten knowledge units to the knowledge bank of the Keepers of Knowledge in the Great Library."},
			{"branch",
				{"choice", "What's the location of the Great Library?",
					{"say", "Library security enforcer", "The location of the Great Library is..."},
					{"say", "Library security enforcer", "*Beep*"},
					{"say", "Library security enforcer", "...known. You may purchase the information for one knowledge unit."},
					{"say", "Library security enforcer", "The Keepers of Knowledge have deemed one volume of \"Arura Research Diary\" a sufficient payment."},
					{"branch",
						{"choice", "Deal, \"Arura Research Diary\" for the location of the Great Library.",
							{"remove player item", "Arura Research Diary",
								{"branch",
									{"say", "Library security enforcer", "I have obtained and validated the payment."},
									{"branch", cond = "great library known",
										{"say", "Library security enforcer", "Your permission to access the knowledge has been renewed."},
									},
									{"branch", cond_not = "great library known",
										{"say", "Library security enforcer", "I have marked the location of the Great Library in your map."},
										{"flag", "great library known"},
										{"quest", "Keepers of Knowledge", marker = "great library", status = "active", text = "We have purchased the location of the Great Library from the security enforcer of the sealed library. To gain access to the sealed library, a payment of ten knowledge units must be paid to the Keepers of Knowledge."}
									},
									{"break", 4}
								},
								{"say", "Library security enforcer", "You need one more knowledge unit to obtain the information."}
							}
						},
						{"choice", "Why should I pay to know how to pay? Ridiculous!",
							{"say", "Library security enforcer", "All knowledge is the property of the Keepers of Knowledge."},
							{"say", "Library security enforcer", "All rights are reserved."},
							{"say", "Library security enforcer", "A temporary permission to access a specific bit of information may be granted for a sufficient payment."},
							{"say", "Library security enforcer", "The permission may be revoked at any time without notification."},
							{"say", "Library security enforcer", "The punishment for each unauthorized use of knowledge is death."}
						},
						{"choice", "Nevermind.",
							{"break", 2}
						},
						{"loop"}
					},
				},
				{"choice", "What do you mean with a \"knowledge unit\"?",
					{"say", "Library security enforcer", "Knowledge is the currency of the Keepers of Knowledge."},
					{"say", "Library security enforcer", "A knowledge unit is a fact or collection of facts of interest to the Keepers of Knowledge."},
					{"say", "Library security enforcer", "A knowledge unit may be a physical volume of information or a series of verbally delivered facts ordered by the Keepers of Knowledge."},
				},
				{"choice", "What if I just break inside without paying?",
					{"say", "Library security enforcer", "Unauthorized access to knowledge is punishable by death."},
					{"say", "Library security enforcer", "Intruders will be shot in sight."}
				},
				{"choice", "Let's talk about something else.",
					{"break", 2}
				},
				{"loop"}
			}
		},
		{"choice", "Goodbye.",
			{"break", 2}
		},
		{"loop"}
	}
}}
