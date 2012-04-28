Program:add_path(Mod.path)

Obstaclespec{
	name = "portal of lips",
	model = "portal1",
	categories = {["portal"] = true, ["special"] = true},
	dialog = "portal of lips",
	marker = "portal of lips"}

Obstaclespec{
	name = "portal of midguard",
	model = "portal1",
	categories = {["portal"] = true, ["special"] = true},
	dialog = "portal of midguard",
	marker = "portal of midguard"}

require(Mod.path .. "portaloflips")
require(Mod.path .. "portaloflips1")
require(Mod.path .. "portalofmidguard")
require(Mod.path .. "portalofmidguard1")
