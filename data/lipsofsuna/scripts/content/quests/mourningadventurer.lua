Quest{
	name = "Mourning Adventurer",
	status = "inactive"}

Pattern{
	name = "mourning adventurer lost",
	creatures = {
		{0,0,0,"lost adventurer"},
		{2,0,0,"dragon"}}}

Species{
	name = "mourning adventurer",
	base = "aer",
	dialog = "mourning adventurer town",
	marker = "mourning adventurer town",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"neutral"}}

Species{
	name = "lost adventurer",
	base = "aer",
	dialog = "mourning adventurer lost",
	marker = "mourning adventurer lost",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"good"}}
