Obstaclespec{
	name = "dungeon entrance(2)",
	model = "portal1",
	categories = {["dungeon entrance"] = true, ["special"] = true},
	dialog = "dungeon entrance(2)",
	marker = "dungeon entrance(2)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon entrance(2)",
	commands = {
		{"teleport", marker = "dungeon exit(2)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon entrance(2)",
	size = Vector(8,8,8),
	categories = {["dungeon entrance"] = true},
	statics = {
		{4,0,4,"dungeon entrance(2)"}}}

------------------------------------------------------------------------------

Obstaclespec{
	name = "dungeon exit(2)",
	model = "portal1",
	categories = {["dungeon exit"] = true, ["special"] = true},
	dialog = "dungeon exit(2)",
	marker = "dungeon exit(2)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon exit(2)",
	commands = {
		{"teleport", marker = "dungeon entrance(2)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon exit(2)",
	size = Vector(8,8,8),
	categories = {["dungeon exit"] = true},
	obstacles = {
		{4,0,4,"dungeon exit(2)"}}}
