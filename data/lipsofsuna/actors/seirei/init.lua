Program:add_path(Mod.path)

Faction{name = "seirei"}

Species{
	name = "seirei",
	model = "seirei1",
	ai_enable_combat = false,
	ai_enable_wander = false,
	factions = {"neutral", "seirei"}}

Species{
	name = "Erinyes",
	base = "seirei",
	dialog = "erinyes",
	marker = "erinyes"}
