Staticspec{
	name = "dungeon entrance(7)",
	model = "portal1",
	categories = {["dungeon entrance"] = true, ["special"] = true},
	dialog = "dungeon entrance(7)",
	marker = "dungeon entrance(7)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon entrance(7)",
	commands = {
		{"teleport", marker = "dungeon exit(7)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon entrance(7)",
	size = Vector(8,8,8),
	categories = {["dungeon entrance"] = true},
	statics = {
		{4,0,4,"dungeon entrance(7)"}}}

------------------------------------------------------------------------------

Obstaclespec{
	name = "dungeon exit(7)",
	model = "portal1",
	categories = {["dungeon exit"] = true, ["special"] = true},
	dialog = "dungeon exit(7)",
	marker = "dungeon exit(7)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon exit(7)",
	commands = {
		{"teleport", marker = "dungeon entrance(7)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon exit(7)",
	size = Vector(8,8,8),
	categories = {["dungeon exit"] = true},
	obstacles = {
		{4,0,4,"dungeon exit(7)"}}}
