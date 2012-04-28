Program:add_path(Mod.path)

Questspec{
	name = "Chara's Illusion"}

Questspec{
	name = "Roots of World"}

Obstaclespec{
	name = "chara's portal",
	model = "portal1",
	categories = {["portal"] = true, ["special"] = true},
	dialog = "chara's portal",
	marker = "chara's portal"}

Actorspec{
	name = "Chara",
	base = "seirei",
	dialog = "chara",
	marker = "chara"}

require(Mod.path .. "chara")
require(Mod.path .. "charasportal")
require(Mod.path .. "portalofillusion")
require(Mod.path .. "charasgrove1")
require(Mod.path .. "charasillusion1")
