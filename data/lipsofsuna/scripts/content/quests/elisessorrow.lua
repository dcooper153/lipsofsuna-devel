Quest{
	name = "Elise's Sorrow",
	status = "inactive"}

Pattern{
	name = "lostadventurer1",
	size = Vector(16,8,16),
	creatures = {
		{5,0,5,"Noemi"},
		{7,0,7,"dragon"}}}

Species{
	name = "Elise",
	base = "aer",
	animations = {
		["idle"] = {animation = "tired", channel = 1, permanent = true}},
	dialog = "elise",
	marker = "elise",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"neutral"}}

Species{
	name = "Noemi",
	base = "aer",
	dialog = "noemi",
	marker = "noemi",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"good"}}
