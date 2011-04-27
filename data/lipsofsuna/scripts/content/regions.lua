Regionspec{
	name = "Lips",
	categories = {"special"},
	depth = {3000,3000},
	links = {"Portal of Lips", "Sanctuary", "Chara's Root Grove"},
	pattern_name = "lips1",
	position = {1536,1536}}

Regionspec{
	name = "Portal of Lips",
	categories = {"special"},
	depth = {2970,3000},
	distance = {"Lips",40,70},
	links = {"Sanctuary", "Chara's Root Grove"},
	pattern_name = "portaloflips1",
	random_resources = false}

Regionspec{
	name = "Sanctuary",
	categories = {"special"},
	depth = {2970,3000},
	distance = {"Lips",60,110},
	pattern_name = "sanctuary1"}

Regionspec{
	name = "Chara's Root Grove",
	categories = {"special"},
	depth = {2930,2950},
	distance = {"Lips",80,120},
	links = {"Midguard"},
	pattern_name = "charasgrove1",
	random_resources = false}

Regionspec{
	name = "Noemi",
	categories = {"special"},
	depth = {2930,2950},
	distance = {"Chara's Root Grove",10,40},
	links = {"Chara's Root Grove"},
	pattern_name = "noemi1"}

Regionspec{
	name = "Midguard",
	categories = {"special"},
	depth = {2700,2800},
	links = {"Portal of Midguard", "Erinyes' Lair"},
	pattern_name = "midguard1"}

Regionspec{
	name = "Portal of Midguard",
	categories = {"special"},
	depth = {2700,2800},
	distance = {"Midguard",100,300},
	pattern_name = "portalofmidguard1"}

Regionspec{
	name = "Erinyes' Portal",
	categories = {"special"},
	depth = {2500,2600},
	pattern_name = "erinyesportal1"}

Regionspec{
	name = "Erinyes' Lair",
	categories = {"special"},
	depth = {32,32},
	pattern_name = "erinyeslair1",
	position = {32,512}}

Regionspec{
	name = "Chara's Illusion",
	categories = {"special"},
	depth = {32,32},
	pattern_name = "charasillusion1",
	position = {32,32}}

Regionspec{
	name = "Random",
	categories = {"random"},
	depth = {2500, 3000},
	pattern_category = "random"}

Regionspec{
	name = "Silverspring",
	categories = {"special"},
	depth = {2900, 3000},
	distance = {"Lips",30,60},
	links = {"Chara's Root Grove", "Portal of Lips", "Sanctuary"},
	pattern_name = "silverspring1"}
