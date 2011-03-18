local regen = 0.5

------------------------------------------------------------------------------
-- Base races.

Species{
	name = "base",
	animations = {
		["attack axe"] = {animation = "attack-axe", channel = 2, weight = 30},
		["attack blunt"] = {animation = "attack-blunt", channel = 2, weight = 30},
		["attack bow"] = {animation = "fire-bow", channel = 2, weight = 30},
		["attack fist"] = {animation = "punch", channel = 2, weight = 30},
		["attack staff"] = {animation = "attack-staff", channel = 2, weight = 30},
		["block start"] = {animation = "block", channel = 2, fade_in = 0.6, fade_out = 0.6, permanent = true, repeat_start = 10, weight = 50},
		["block stop"] = {animation = "block-stop", channel = 2},
		["build"] = {animation = "punch", channel = 2, weight = 30},
		["hold axe"] = {animation = "hold-axe", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold blunt"] = {animation = "hold-blunt", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold bulky"] = {animation = "hold-bulky", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold staff"] = {animation = "hold-staff", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold shield"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5},
		["hold torch"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5},
		["dead"] = {animation = "death", channel = 1, permanent = true, repeat_start = 10, time = 10},
		["death"] = {animation = "death", channel = 1, permanent = true, repeat_start = 10},
		["idle"] = {animation = "idle", channel = 1, permanent = true},
		["run"] = {animation = "run", channel = 1, permanent = true},
		["jump"] = {animation = "jump", channel = 2, fade_out = 0.4, permanent = true, repeat_start = 10, weight = 10},
		["land ground"] = {animation = "land", channel = 2, fade_in = 0.1, fade_out = 0.1, weight = 10},
		["land water"] = {animation = "land", channel = 2},
		["pick up"] = {animation = "pickup", channel = 2, weight = 10},
		["run left"] = {animation = "run-left", channel = 1, permanent = true},
		["run right"] = {animation = "run-right", channel = 1, permanent = true},
		["spell ranged"] = {animation = "cast-spell", channel = 2, weight = 30},
		["spell self"] = {animation = "cast-spell", channel = 2, weight = 30},
		["spell touch"] = {animation = "cast-spell", channel = 2, weight = 30},
		["strafe left"] = {animation = "strafe-left", channel = 1, permanent = true},
		["strafe right"] = {animation = "strafe-right", channel = 1, permanent = true},
		["throw"] = {animation = "throw", channel = 2, weight = 10},
		["throw bulky"] = {animation = "throw-bulky", channel = 2, weight = 10},
		["throw grenade"] = {animation = "throw", channel = 2, weight = 10},
		["walk"] = {animation = "walk", channel = 1, permanent = true},
		["walk back"] = {animation = "walk-back", channel = 1, permanent = true}}}

Species{
	name = "race",
	base = "base",
	aim_ray_center = Vector(0, 1.5, 0),
	aim_ray_end = 5,
	aim_ray_start = 0.5,
	camera_center = Vector(0, 1.5, 0),
	equipment_slots = {
		{name = "arms"},
		{name = "feet"},
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"},
		{name = "head"},
		{name = "lowerbody"},
		{name = "upperbody"}},
	factions = {"good"},
	feat_anims = {"build", "right hand", "ranged", "ranged spell", "spell on self", "spell on touch", "throw"},
	feat_effects = {"black haze", "fire damage", "physical damage", "restore health", "sanctuary"},
	inventory_size = 20,
	skill_quota = 200,
	speed_walk = 2,
	speed_run = 4,
	tilt_bone = "backtilt",
	tilt_limit = 1.25,
	-- Customization.
	body_scale = {0.9, 1.05},
	bust_scale = {0.3, 1.3},
	nose_scale = {0.5, 2.0},
	eye_styles = {
		{"Default", ""}},
	hair_styles = {
		{"Bald", ""},
		{"Pigtails", "hair1"},
		{"Pigtails long", "hair3"},
		{"Long", "hair2"},
		--{"Short", "aershorthair1"} -- TODO: Adapt to head mesh changes
		{"Drill", "hair4"}},
	skin_styles = {
		{"Default", ""}}}

Species{
	name = "aer",
	base = "race",
	effect_falling_damage = "aerhurt1",
	equipment_class = "aer",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		head = "aerhead1",
		hands = "aerhand1",
		feet = "aerfoot1",
		legs = "aerleg1",
		lower = "aerlower1",
		upper = "aerupper1"},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 100, val = 50},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 20},
		{name = "willpower", max = 80, val = 30}}}

Species{
	name = "aermale",
	base = "aer",
	equipment_class = "aermale",
	model = "aermale1",
	models = {
		skeleton = "aermale1",
		arms = "aermale1arms1",
		hands = "aermale1hands1",
		head = "aermale1head1",
		legs = "aermale1legs1",
		feet = "aermale1feet1",
		eyes = "aermale1eyes1",
		upper = "aermale1chest1"},
	hair_styles = {
		{"Bald", ""},
		{"Short", "aermale1hair1"}},
	timing_attack_throw = 4,
	timing_spell_ranged = 4,
	timing_spell_touch = 4}

Species{
	name = "android",
	base = "race",
	damage_from_magma = 2,
	damage_from_water = 5,
	equipment_class = "android",
	model = "android1",
	models = {
		skeleton = "android1",
		arms = "androidarm1",
		feet = "androidfoot1",
		hand = "androidhand1",
		head = "androidhead1",
		legs = "androidleg1",
		lower = "androidlower1",
		upper = "androidupper1"},
	skills = {
		{name = "dexterity", max = 60, val = 20},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 80, val = 30},
		{name = "perception", max = 100, val = 50},
		{name = "strength", max = 80, val = 30},
		{name = "willpower", max = 40, val = 20}}}

-- TODO
Species{
	name = "androidmale",
	base = "android",
	equipment_class = "aermale",
	model = "aermale1",
	models = {
		skeleton = "aermale1",
		arms = "aermale1arms1",
		hands = "aermale1hands1",
		head = "aermale1head1",
		legs = "aermale1legs1",
		feet = "aermale1feet1",
		eyes = "aermale1eyes1",
		upper = "aermale1chest1"},
	hair_styles = {
		{"Short", "aermale1hair1"}}}

Species{
	name = "devora",
	base = "race",
	aim_ray_center = Vector(0, 1, 0),
	camera_center = Vector(0, 1, 0),
	damage_from_magma = 9,
	equipment_class = "devora",
	hair_styles = {
		{"Bald", ""},
		{"Plant 1", "devorahair1"},
		{"Plant 2", "devorahair2"}},
	model = "devora1",
	models = {
		skeleton = "devora1",
		arms = "devoraarmnude1",
		head = "devorahead1",
		legs = "devoralegnude1",
		lower = "devoralowernude1",
		upper = "devorauppernude1"},
	models_lod = {
		skeleton = "aer1",
		arms = "aerarmnude1l",
		head = "aerhead1l",
		legs = "aerlegnude1l",
		lower = "aerlowernude1l",
		upper = "aeruppernude1l"},
	skills = {
		{name = "dexterity", max = 100, val = 50},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 40, val = 20},
		{name = "perception", max = 80, val = 30},
		{name = "strength", max = 60, val = 20},
		{name = "willpower", max = 80, val = 30}}}

Species{
	name = "kraken",
	base = "race",
	damage_from_magma = 9,
	damage_from_water = -2,
	equipment_class = "kraken",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		head = "aerhead1",
		hands = "aerhand1",
		feet = "aerfoot1",
		legs = "aerleg1",
		lower = "aerlower1",
		upper = "aerupper1"},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 60, val = 20},
		{name = "perception", max = 40, val = 20},
		{name = "strength", max = 80, val = 30},
		{name = "willpower", max = 100, val = 50}},
	water_friction = 0}

Species{
	name = "wyrm",
	base = "race",
	damage_from_magma = 0,
	equipment_class = "wyrm",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		head = "aerhead1",
		hands = "aerhand1",
		feet = "aerfoot1",
		legs = "aerleg1",
		lower = "aerlower1",
		upper = "aerupper1"},
	skills = {
		{name = "dexterity", max = 40, val = 20},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 80, val = 30},
		{name = "perception", max = 80, val = 30},
		{name = "strength", max = 100, val = 50},
		{name = "willpower", max = 60, val = 20}}}

-- TODO
Species{
	name = "wyrmmale",
	base = "wyrm",
	equipment_class = "aermale",
	model = "aermale1",
	models = {
		skeleton = "aermale1",
		arms = "aermale1arms1",
		hands = "aermale1hands1",
		head = "aermale1head1",
		legs = "aermale1legs1",
		feet = "aermale1feet1",
		eyes = "aermale1eyes1",
		upper = "aermale1chest1"},
	hair_styles = {
		{"Short", "aermale1hair1"}}}

Species{
	name = "trogfemale",
	base = "race",
	model = "trogfem1",
	models = {
		skeleton = "trogfem1",
		arms = "trogfemarmnude1",
		head = "trogfemhead1",
		legs = "trogfemlegnude1",
		lower = "trogfemlowernude1",
		upper = "trogfemuppernude1"},
	skills = {
		{name = "dexterity", max = 8, val = 3},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 10, val = 5},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 4, val = 2},
		{name = "willpower", max = 8, val = 3}},
	hair_styles = {	
		{"troglong", "trogfemlonghair1"},
		{"trogshort", "trogfemshorthair1"}},
	skin_styles = {
		{""}}}

------------------------------------------------------------------------------
-- Playable races.

Species{
	name = "aer-player",
	base = "aer",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "dress", "leather leggings"}}

Species{
	name = "aermale-player",
	base = "aermale",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "shirt", "pants", "leather boots"}}

Species{
	name = "android-player",
	base = "android",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "dress", "leather leggings"}}

Species{
	name = "androidmale-player",
	base = "androidmale",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "shirt", "pants", "leather boots"}}

Species{
	name = "devora-player",
	base = "devora",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "dress", "leather leggings"}}

Species{
	name = "kraken-player",
	base = "kraken",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "dress", "leather leggings"}}

Species{
	name = "wyrm-player",
	base = "wyrm",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "dress", "leather leggings"}}

Species{
	name = "wyrmmale-player",
	base = "wyrmmale",
	ai_enabled = false,
	inventory_items = {"torch", "workbench", "shirt", "pants", "leather boots"}}

------------------------------------------------------------------------------
-- Friendly creatures.

Species{
	name = "lipscitizen",
	base = "aer",
	dialog = "lipscitizen",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather top", "leather pants", "leather leggings"}}

Species{
	name = "lipscitizenmale",
	base = "aermale",
	dialog = "lipscitizen",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather chest", "leather pants", "leather boots", "leather gloves", "leather helm"}}

Species{
	name = "seirei",
	model = "seirei1",
	ai_enable_combat = false,
	ai_enable_wander = false,
	factions = {"neutral", "seirei"}}

------------------------------------------------------------------------------
-- Enemies.

Species{
	name = "brigand",
	base = "aer",
	categories = {"enemy"},
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron sword", "leather top", "leather pants", "leather leggings"},
	loot_categories = {"armor", "material", "potion", "shield", "weapon"},
	loot_count = {2, 4}}

Species{
	name = "brigandmale",
	base = "aermale",
	categories = {"enemy"},
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather brigand chest", "leather brigand pants", "leather boots", "leather gloves", "leather helm"},
	loot_categories = {"armor", "material", "potion", "shield", "weapon"},
	loot_count = {2, 4}}

Species{
	name = "troglodyte female",
	base = "trogfemale",
	categories = {"enemy"},
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"wooden club", "trog rags"},
	loot_categories = {"material"},
	loot_count = {2, 4}}

Species{
	name = "bloodworm",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_defend = false,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy"},
	factions = {"evil"},
	feat_anims = {"bite"},
	inventory_size = 3,
	inventory_items = {"hide", "hide"},
	jump_force = 3,
	model = "bloodworm2",
	speed_walk = 2,
	skills = {
		{name = "health", max = 30, val = 30}}}

Species{
	name = "dragon",
	base = "base",
	model = "dragon1",
	ai_distance_hint = 2.5,
	ai_enable_strafe = false,
	ai_enable_defend = false,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30},
		["attack left claw"] = {animation = "claw-left", channel = 2, weight = 30},
		["attack right claw"] = {animation = "claw-right", channel = 2, weight = 30}},
	factions = {"evil"},
	feat_anims = {"dragon bite", "left claw", "right claw"},
	inventory_size = 10,
	inventory_items = {"dragon scale", "dragon scale", "dragon scale", "dragon scale", "dragon scale",
		"adamantium", "adamantium", "adamantium", "adamantium", "crimson stone", "crimson stone"},
	mass = 1000,
	speed_walk = 1,
	skills = {
		{name = "health", max = 100, val = 100}}}

Species{
	name = "feral devora",
	base = "devora",
	categories = {"enemy"},
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"},
	loot_categories = {"material"},
	loot_count = {2, 4}}

Species{
	name = "lizardman",
	base = "base",
	model = "lizardman2",
	ai_distance_hint = 1.0,
	ai_enable_defend = false,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy"},
	factions = {"evil"},
	feat_anims = {"right hand","bite"},
	inventory_size = 5,
	inventory_items = {"wooden club", "lizard scale", "lizard scale", "lizard scale"},
	loot_categories = {"material"},
	loot_count = {2, 4},
	mass = 100,
	speed_walk = 3,
	equipment_slots = {
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"}},
	skills = {
		{name = "dexterity", max = 5, val = 5},
		{name = "health", max = 50, val = 50},
		{name = "strength", max = 5, val = 5}}}

Species{
	name = "plague rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_defend = false,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy", "plague"},
	factions = {"evil"},
	feat_anims = {"bite"},
	inventory_size = 3,
	inventory_items = {"hide", "hide"},
	jump_force = 6,
	model = "plaguerat1", -- FIXME
	speed_walk = 4,
	skills = {
		{name = "health", max = 30, val = 30}}}

Species{
	name = "plague slime",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_defend = false,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy", "plague"},
	factions = {"evil"},
	feat_anims = {"bite"},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2", -- FIXME
	speed_walk = 2,
	skills = {
		{name = "health", max = 30, val = 30}}}

Species{
	name = "stone imp",
	base = "base",
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	ai_enable_defend = false,
	categories = {"enemy", "mining"},
	factions = {"evil"},
	feat_anims = {"bite"},
	inventory_size = 2,
	equipment_slots = {
		{name = "hand.R", node = "#hand.R"},
		{name = "hand.L", node = "#hand.L"}},
	inventory_items = {"impstone"},
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	skills = {
		{name = "health", max = 20, val = 20}}}

Species{
	name = "fire imp",
	base = "base",
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	ai_enable_defend = false,
	categories = {"enemy", "mining"},
	factions = {"evil"},
	feat_anims = {"bite"},
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	inventory_size = 2,
	equipment_slots = {
		{name = "hand.R", node = "#hand.R"},
		{name = "hand.L", node = "#hand.L"}},
	inventory_items = {"firestone"},
	skills = {
		{name = "health", max = 20, val = 20}}}

------------------------------------------------------------------------------
-- Silverspring creatures.

Species{
	name = "The Hoarder",
	base = "aermale",
	dialog = "the hoarder",
	marker = "the hoarder",
	eye_style = {"default",255,32,32},
	hair_style = {"hair2",200,20,180},
	inventory_items = {"shirt", "pants", "boots"}}
