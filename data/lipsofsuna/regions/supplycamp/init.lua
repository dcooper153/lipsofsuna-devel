Program:add_path(Mod.path)

Staticspec{
	name = "entrance to suna dungeons",
	model = "portal1",
	categories = {["portal"] = true, ["special"] = true},
	dialog = "entrance to suna dungeons",
	marker = "entrance to suna dungeons",
	usages = {["teleport"] = true}}

Dialogspec{name = "entrance to suna dungeons", commands = {
	{"teleport", marker = "lyra"}}}

require(Mod.path .. "guy")
require(Mod.path .. "supplycamp1")
