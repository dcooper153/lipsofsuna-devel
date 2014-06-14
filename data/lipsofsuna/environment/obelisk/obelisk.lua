local DialogSpec = require("core/specs/dialog")

DialogSpec{name = "obelisk", commands = {
	{"branch", check = {{"var", "examined"}},
		{"effect", "obelisk visit 2"},
		{"info", "As you examine the already familiar obelisk, you are reminded of your past discoveries."},
		{"exit"}
	},
	{"effect", "obelisk visit 1"},
	{"random",
		{"info", "Upon examining the unfamiliar obelisk, its strange runes coalesce into a grand vision in your mind."},
		{"info", "Upon examining the unfamiliar obelisk, its strange runes paint in your mind an image of yourself standing on the shoulder of the giant."},
		{"info", "Upon examining the unfamiliar obelisk, its strange runes sing to you in the language of the muses."},
		{"info", "Upon examining the unfamiliar obelisk, its strange runes show you a glimpse of the secrets of the aether."},
		{"info", "Upon examining the unfamiliar obelisk, its strange runes smile upon you as if no words need to be said."}
	},
	{"var", "examined"},
	{"unlock reward"}}}
