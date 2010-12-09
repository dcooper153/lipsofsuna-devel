Quest{
	name = "Mourning Adventurer",
	status = "inactive"}

Pattern{
	name = "mourningadventurer_town",
	creatures = {
		{0,0,0,"mourning adventurer"}}}

Pattern{
	name = "mourningadventurer_lost",
	creatures = {
		{0,0,0,"lost adventurer"},
		{2,0,0,"dragon"}}}

Species{
	name = "mourning adventurer",
	base = "aer",
	dialog = "mourningadventurer_townchar",
	marker = "mourningadventurer_townchar",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"axe", "dress"},
	factions = {"neutral"}}

Species{
	name = "lost adventurer",
	base = "aer",
	dialog = "mourningadventurer_lostchar",
	marker = "mourningadventurer_lostchar",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"axe", "dress"},
	factions = {"good"}}
