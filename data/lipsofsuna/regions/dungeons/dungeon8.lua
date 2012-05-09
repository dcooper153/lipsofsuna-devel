Staticspec{
	name = "dungeon entrance(8)",
	model = "portal1",
	categories = {["dungeon entrance"] = true, ["special"] = true},
	dialog = "dungeon entrance(8)",
	marker = "dungeon entrance(8)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon entrance(8)",
	commands = {
		{"teleport", marker = "dungeon exit(8)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon entrance(8)",
	size = Vector(8,8,8),
	categories = {["dungeon entrance"] = true},
	statics = {
		{4,0,4,"dungeon entrance(8)"}}}

------------------------------------------------------------------------------

Obstaclespec{
	name = "dungeon exit(8)",
	model = "portal1",
	categories = {["dungeon exit"] = true, ["special"] = true},
	dialog = "dungeon exit(8)",
	marker = "dungeon exit(8)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon exit(8)",
	commands = {
		{"teleport", marker = "dungeon entrance(8)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon exit(8)",
	size = Vector(8,8,8),
	categories = {["dungeon exit"] = true},
	obstacles = {
		{4,0,4,"dungeon exit(8)"}}}
