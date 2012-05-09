Staticspec{
	name = "dungeon entrance(6)",
	model = "portal1",
	categories = {["dungeon entrance"] = true, ["special"] = true},
	dialog = "dungeon entrance(6)",
	marker = "dungeon entrance(6)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon entrance(6)",
	commands = {
		{"teleport", marker = "dungeon exit(6)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon entrance(6)",
	size = Vector(8,8,8),
	categories = {["dungeon entrance"] = true},
	statics = {
		{4,0,4,"dungeon entrance(6)"}}}

------------------------------------------------------------------------------

Obstaclespec{
	name = "dungeon exit(6)",
	model = "portal1",
	categories = {["dungeon exit"] = true, ["special"] = true},
	dialog = "dungeon exit(6)",
	marker = "dungeon exit(6)",
	usages = {["teleport"] = true}}

Dialogspec{
	name = "dungeon exit(6)",
	commands = {
		{"teleport", marker = "dungeon entrance(6)"},
		{"effect player", "portal1"},
		{"exit"}}}

Patternspec{
	name = "dungeon exit(6)",
	size = Vector(8,8,8),
	categories = {["dungeon exit"] = true},
	obstacles = {
		{4,0,4,"dungeon exit(6)"}}}
