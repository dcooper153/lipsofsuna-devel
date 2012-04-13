Program:add_path(Mod.path)

Quest{name = "Chara's Illusion"}
Quest{name = "Roots of World"}

Obstaclespec{
	name = "chara's portal",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "chara's portal",
	marker = "chara's portal"}

Species{
	name = "Chara",
	base = "seirei",
	dialog = "chara",
	marker = "chara"}

require(Mod.path .. "chara")
require(Mod.path .. "charasportal")
require(Mod.path .. "portalofillusion")
require(Mod.path .. "charasgrove1")
require(Mod.path .. "charasillusion1")
