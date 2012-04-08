Program:add_path(Mod.path)

Regionspec{
	name = "Supply Camp",
	categories = {"special"},
	depth = {1300,1300},
	pattern_name = "supplycamp1",
	position = {940,1050},
	spawn_point = {5,5,5}}

Staticspec{
	name = "entrance to suna dungeons",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "entrance to suna dungeons",
	marker = "entrance to suna dungeons"}

Dialogspec{name = "entrance to suna dungeons",
	{"teleport", marker = "lyra"}}

require(Mod.path .. "supplycamp1")
