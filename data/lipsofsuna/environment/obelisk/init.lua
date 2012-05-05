Program:add_path(Mod.path)

Staticspec{
	name = "obelisk",
	model = "obelisk1",
	categories = {["obelisk"] = true, ["special"] = true},
	dialog = "obelisk",
	usages = {["examine"] = true}}

require(Mod.path .. "obelisk")
