Program:add_path(Mod.path)

Actorspec{
	name = "Foreman",
	base = "aer",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "foreman",
	marker = "foreman",
	eye_style = "random",
	hair_style = "random",
	important = true,
	inventory_items = {["dress"] = 1}}

Actorspec{
	name = "Noemi",
	base = "aer",
	dialog = "noemi",
	marker = "noemi",
	dead = true,
	hair_color = {200,180,20},
	hair_style = "hair2",
	important = true,
	inventory_items = {["iron hatchet"] = 1, ["dress"] = 1},
	factions = {["good"] = true}}

require(Mod.path .. "effects")
require(Mod.path .. "items")
require(Mod.path .. "obstacles")
require(Mod.path .. "quests")
require(Mod.path .. "dialogs/bridgechest")
require(Mod.path .. "dialogs/erinyes")
require(Mod.path .. "dialogs/foreman")
require(Mod.path .. "dialogs/noemi")
require(Mod.path .. "dialogs/sanctuary")
require(Mod.path .. "patterns/brigandhq1")
require(Mod.path .. "patterns/chasm1")
require(Mod.path .. "patterns/erinyeslair1")
require(Mod.path .. "patterns/erinyesportal1")
require(Mod.path .. "patterns/midguard1")
require(Mod.path .. "patterns/noemi1")
require(Mod.path .. "patterns/ruins1")
require(Mod.path .. "patterns/ruins2")
require(Mod.path .. "patterns/ruins3")
require(Mod.path .. "patterns/sanctuary1")
