Program:add_path(Mod.path)

Staticspec{
	name = "entrance to suna dungeons",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "entrance to suna dungeons",
	marker = "entrance to suna dungeons"}

Dialogspec{name = "entrance to suna dungeons",
	{"teleport", marker = "lyra"}}

require(Mod.path .. "guy")
require(Mod.path .. "supplycamp1")
