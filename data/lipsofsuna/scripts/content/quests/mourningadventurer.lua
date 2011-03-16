Quest{
	name = "Mourning Adventurer",
	status = "inactive"}

Pattern{
	name = "lostadventurer1",
	size = Vector(16,8,16),
	creatures = {
		{5,0,5,"lost adventurer"},
		{7,0,7,"dragon"}}}

Species{
	name = "mourning adventurer",
	base = "aer",
	animations = {
		["idle"] = {animation = "tired", channel = 1, permanent = true}},
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
