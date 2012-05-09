Staticspec{
	name = "dungeon entrance(3)",
	model = "portal1",
	categories = {["dungeon entrance"] = true, ["special"] = true},
	dialog = "dungeon entrance(3)",
	marker = "dungeon entrance(3)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon entrance(3)",
	commands = {
		{"teleport", marker = "dungeon exit(3)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon entrance(3)",
	size = Vector(8,8,8),
	categories = {["dungeon entrance"] = true},
	statics = {
		{4,0,4,"dungeon entrance(3)"}}}

------------------------------------------------------------------------------

Obstaclespec{
	name = "dungeon exit(3)",
	model = "portal1",
	categories = {["dungeon exit"] = true, ["special"] = true},
	dialog = "dungeon exit(3)",
	marker = "dungeon exit(3)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon exit(3)",
	commands = {
		{"teleport", marker = "dungeon entrance(3)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon exit(3)",
	size = Vector(8,8,8),
	categories = {["dungeon exit"] = true},
	obstacles = {
		{4,0,4,"dungeon exit(3)"}}}
