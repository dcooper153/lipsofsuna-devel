Staticspec{
	name = "dungeon entrance(5)",
	model = "portal1",
	categories = {["dungeon entrance"] = true, ["special"] = true},
	dialog = "dungeon entrance(5)",
	marker = "dungeon entrance(5)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon entrance(5)",
	commands = {
		{"teleport", marker = "dungeon exit(5)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon entrance(5)",
	size = Vector(8,8,8),
	categories = {["dungeon entrance"] = true},
	statics = {
		{4,0,4,"dungeon entrance(5)"}}}

------------------------------------------------------------------------------

Obstaclespec{
	name = "dungeon exit(5)",
	model = "portal1",
	categories = {["dungeon exit"] = true, ["special"] = true},
	dialog = "dungeon exit(5)",
	marker = "dungeon exit(5)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon exit(5)",
	commands = {
		{"teleport", marker = "dungeon entrance(5)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon exit(5)",
	size = Vector(8,8,8),
	categories = {["dungeon exit"] = true},
	obstacles = {
		{4,0,4,"dungeon exit(5)"}}}
