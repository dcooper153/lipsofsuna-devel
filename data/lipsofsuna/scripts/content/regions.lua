Regionspec{
	name = "Lips",
	categories = {"special"},
	depth = {1000,1000},
	links = {"Portal of Lips", "Sanctuary", "Chara's Root Grove"},
	pattern_name = "lips1",
	position = {700,700}}

Regionspec{
	name = "Giant Shaft",
	categories = {"special"},
	depth = {930,1000},
	links = {"Lips", "Sanctuary"},
	pattern_name = "giantshaft"}

Regionspec{
	name = "Chasm",
	categories = {"special"},
	depth = {930,1000},
	links = {"Giant Shaft"},
	pattern_name = "chasm1"}

Regionspec{
	name = "Brigand HQ",
	categories = {"special"},
	depth = {930,930},
	distance = {"Lips",10,20},
	pattern_name = "brigandhq1",
	random_resources = false}

Regionspec{
	name = "Portal of Lips",
	categories = {"special"},
	depth = {930,1000},
	distance = {"Lips",20,40},
	links = {"Sanctuary", "Chara's Root Grove"},
	pattern_name = "portaloflips1",
	random_resources = false}

Regionspec{
	name = "Sanctuary",
	categories = {"special"},
	depth = {930,1000},
	distance = {"Lips",30,60},
	pattern_name = "sanctuary1"}

Regionspec{
	name = "Chara's Root Grove",
	categories = {"special"},
	depth = {950,970},
	distance = {"Lips",40,60},
	links = {"Midguard"},
	pattern_name = "charasgrove1",
	random_resources = false}

Regionspec{
	name = "Noemi",
	categories = {"special"},
	depth = {950,970},
	distance = {"Chara's Root Grove",10,40},
	links = {"Chara's Root Grove"},
	pattern_name = "noemi1"}

Regionspec{
	name = "Midguard",
	categories = {"special"},
	depth = {800,900},
	links = {"Portal of Midguard", "Erinyes' Lair"},
	pattern_name = "midguard1"}

Regionspec{
	name = "Portal of Midguard",
	categories = {"special"},
	depth = {800,900},
	distance = {"Midguard",50,150},
	pattern_name = "portalofmidguard1"}

Regionspec{
	name = "Erinyes' Portal",
	categories = {"special"},
	depth = {500,600},
	pattern_name = "erinyesportal1"}

Regionspec{
	name = "Erinyes' Lair",
	categories = {"special"},
	depth = {128,128},
	pattern_name = "erinyeslair1",
	position = {128,512}}

Regionspec{
	name = "Chara's Illusion",
	categories = {"special"},
	depth = {128,128},
	pattern_name = "charasillusion1",
	position = {128,128}}

Regionspec{
	name = "Silverspring",
	categories = {"special"},
	depth = {900, 1000},
	distance = {"Lips",30,60},
	links = {"Chara's Root Grove", "Portal of Lips", "Sanctuary"},
	pattern_name = "silverspring1"}
