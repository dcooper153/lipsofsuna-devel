Quest{
	name = "Peculiar Pet",
	status = "inactive"}

Pattern{
	name = "peculiarpet",
	creatures = {
		{0,0,0,"peculiarpet_worm"},
		{1.3,0,0,"peculiarpet_npc"}}}

Species{
	name = "peculiarpet_npc",
	base = "aer",
	dialog = "peculiarpet_npc",
	marker = "peculiarpet_npc",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair1",0,255,0},
	inventory_items = {"axe", "dress"},
	factions = {"peculiarpet"}}

Species{
	name = "peculiarpet_worm",
	base = "bloodworm",
	dialog = "peculiarpet_worm",
	marker = "peculiarpet_worm",
	ai_enable_combat = false,
	ai_enable_wander = false,
	factions = {"peculiarpet"}}
