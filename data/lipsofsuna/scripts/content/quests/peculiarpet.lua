Quest{
	name = "Peculiar Pet",
	status = "inactive"}

Pattern{
	name = "peculiar pet",
	creatures = {
		{0,0,0,"peculiar pet worm"},
		{1.3,0,0,"peculiar pet npc"}}}

Species{
	name = "peculiar pet npc",
	base = "aer",
	dialog = "peculiar pet npc",
	marker = "peculiar pet npc",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair1",0,255,0},
	inventory_items = {"axe", "dress"},
	factions = {"peculiar pet"}}

Species{
	name = "peculiar pet worm",
	base = "bloodworm",
	categories = {},
	dialog = "peculiar pet worm",
	marker = "peculiar pet worm",
	ai_enable_combat = false,
	ai_enable_wander = false,
	factions = {"peculiar pet"}}
