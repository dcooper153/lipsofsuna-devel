Regionspec{
	name = "Lips",
	categories = {"special"},
	depth = {3000,3000},
	links = {"Portal of Lips", "Sanctuary", "Chara's Root Grove"},
	pattern_name = "lips1",
	position = {1536,1536},
	size = {32,16,32}}

Regionspec{
	name = "Portal of Lips",
	categories = {"special"},
	depth = {2970,3000},
	distance = {"Lips",40,50},
	links = {"Sanctuary", "Chara's Root Grove"},
	random_resources = false,
	size = {16,8,16}}

Regionspec{
	name = "Sanctuary",
	categories = {"special"},
	depth = {2970,3000},
	distance = {"Lips",40,70},
	size = {20,15,26}}

Regionspec{
	name = "Chara's Root Grove",
	categories = {"special"},
	depth = {2930,2950},
	distance = {"Lips",80,100},
	links = {"Midguard"},
	pattern_name = "charasgrove1",
	size = {16,8,16}}

Regionspec{
	name = "Lost Adventurer",
	categories = {"special"},
	depth = {2930,2950},
	distance = {"Chara's Root Grove",10,20},
	links = {"Chara's Root Grove"},
	pattern_name = "lostadventurer1",
	size = {16,8,16}}

Regionspec{
	name = "Midguard",
	categories = {"special"},
	depth = {2700,2800},
	links = {"Portal of Midguard", "Erinyes' Lair"},
	size = {48,16,48}}

Regionspec{
	name = "Portal of Midguard",
	categories = {"special"},
	depth = {2700,2800},
	distance = {"Midguard",100,300},
	size = {16,8,16}}

Regionspec{
	name = "Erinyes' Portal",
	categories = {"special"},
	depth = {2500,2600},
	size = {16,8,16}}

Regionspec{
	name = "Erinyes' Lair",
	categories = {"special"},
	depth = {32,32},
	position = {32,512},
	size = {32,16,32}}

Regionspec{
	name = "Chara's Illusion",
	categories = {"special"},
	depth = {32,32},
	position = {32,32},
	size = {16,8,16}}

Regionspec{
	name = "Random: Dungeon",
	categories = {"random"},
	depth = {2500, 3000},
	pattern_category = "dungeon",
	size = {8,8,8}}

Regionspec{
	name = "Random: Nature",
	categories = {"random"},
	depth = {2500, 3000},
	pattern_category = "nature",
	size = {8,8,8}}

Regionspec{
	name = "Silverspring",
	categories = {"special"},
	depth = {2900, 3000},
	distance = {"Lips",30,60},
	links = {"Chara's Root Grove", "Portal of Lips", "Sanctuary"},
	pattern_name = "silverspring1",
	size = {32,16,32}}
