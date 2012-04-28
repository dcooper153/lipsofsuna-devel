 Dialogspec{name = "castlewanted", commands = {
	{"branch",cond_not="castle known",
		{"say", "Castle Builder Wanted Poster","Are you an enterprising Adventurer? The Archcouncil of Guilds requires a castle be built to help ward off the hordes of dungeon denizens."},
		{"say", "Castle Builder Wanted Poster","Below is a map of the designated location, more instructions will be given upon arrival"},
		{"flag","castle known"},
		{"quest", "My Little Castle", status = "active", marker = "castleforeman", text = "A castle needs to be built somewhere in the dungeons, according to a poster in town"},{"exit"}},	
	{"branch",cond="castle known",
		{"info","You see another Castle Builder Wanted Poster"},
		{"exit"}},
		{"loop"}}}
