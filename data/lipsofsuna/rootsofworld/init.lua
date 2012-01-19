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

Regionspec{
	name = "Chara's Illusion",
	categories = {"special"},
	depth = {128,128},
	pattern_name = "charasillusion1",
	position = {128,128}}

Regionspec{
	name = "Chara's Root Grove",
	categories = {"special"},
	depth = {950,970},
	distance = {"Lips",40,60},
	links = {"Midguard"},
	pattern_name = "charasgrove1",
	random_resources = false}

require(Mod.path .. "chara")
require(Mod.path .. "charasportal")
require(Mod.path .. "portalofillusion")
require(Mod.path .. "charasgrove1")
require(Mod.path .. "charasillusion1")
