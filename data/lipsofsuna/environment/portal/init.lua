Program:add_path(Mod.path)

Regionspec{
	name = "Portal of Lips",
	categories = {"special"},
	depth = {1000,1000},
	distance = {"Lips",10,40},
	pattern_name = "portaloflips1"}

Regionspec{
	name = "Portal of Midguard",
	categories = {"special"},
	depth = {900,900},
	distance = {"Midguard",10,40},
	pattern_name = "portalofmidguard1"}

Obstaclespec{
	name = "portal of lips",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "portal of lips",
	marker = "portal of lips"}

Obstaclespec{
	name = "portal of midguard",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "portal of midguard",
	marker = "portal of midguard"}

require(Mod.path .. "portaloflips")
require(Mod.path .. "portaloflips1")
require(Mod.path .. "portalofmidguard")
require(Mod.path .. "portalofmidguard1")
