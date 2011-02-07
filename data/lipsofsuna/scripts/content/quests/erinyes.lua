Quest{
	name = "Erinyes",
	status = "inactive"}

Obstaclespec{
	name = "erinyes' portal",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "erinyes' portal",
	marker = "erinyes' portal"}

Pattern{
	name = "erinyes' lair",
	creatures = {
		{3,3,3,"Erinyes"}},
	obstacles = {
		{4,0,4,"erinyes' portal"}}}

Species{
	name = "Erinyes",
	base = "seirei",
	dialog = "erinyes",
	marker = "erinyes"}
