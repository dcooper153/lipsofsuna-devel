require "content/factions"

------------------------------------------------------------------------------
-- Base races.

Species{
	name = "base",
	animations = {
		["attack axe"] = {animation = "attack-axe", channel = 2, fade_in = 0.1, weight = 100},
		["attack blunt"] = {animation = "attack-blunt", channel = 2, fade_in = 0.1, weight = 100},
		["attack bow"] = {animation = "attack-bow", channel = 2, weight = 100},
		["attack crossbow"] = {animation = "attack-crossbow", channel = 2, weight = 5000},
		["attack punch"] = {animation = "attack-punch", channel = 2, fade_out = 0.1, weight = 100},
		["attack musket"] = {animation = "attack-musket", channel = 2, weight = 5000},
		["attack revolver"] = {animation = "attack-revolver", channel = 2, weight = 5000},
		["attack sword"] = {animation = "attack-sword", channel = 2, weight = 100},
		["attack staff"] = {animation = "attack-staff", channel = 2, weight = 100},
		["block start"] = {animation = "block", channel = 2, fade_in = 0.6, fade_out = 0.6, permanent = true, repeat_start = 10, weight = 50},
		["block stop"] = {animation = "block-stop", channel = 2},
		["build"] = {animation = "punch", channel = 2, weight = 30},
		["charge axe"] = {animation = "charge-axe", channel = 2, fade_in = 0.1, permanent = true, repeat_start = 10, weight = 100},
		["charge blunt"] = {animation = "charge-blunt", channel = 2, fade_in = 0.1, permanent = true, repeat_start = 10, weight = 100},
		["charge bow"] = {animation = "charge-bow", channel = 2, permanent = true, repeat_start = 10, weight = 100},
		["charge cancel"] = {channel = 2},
		["charge crossbow"] = {animation = "charge-crossbow", channel = 2, permanent = true, repeat_start = 10, weight = 5000},
		["charge grenade"] = {animation = "charge-throw", channel = 2, permanent = true, repeat_start = 10, weight = 100},
		["charge musket"] = {animation = "charge-musket", channel = 2, permanent = true, repeat_start = 10, weight = 5000},
		["charge punch"] = {animation = "charge-punch", channel = 2, fade_out = 0.1, permanent = true, repeat_start = 10, weight = 100},
		["charge revolver"] = {animation = "charge-revolver", channel = 2, permanent = true, repeat_start = 10, weight = 5000},
		["charge sword"] = {animation = "charge-blunt", channel = 2, permanent = true, repeat_start = 10, weight = 100},
		["charge staff"] = {animation = "charge-axe", channel = 2, permanent = true, repeat_start = 10, weight = 100},
		["climb high"] = {animation = "climb-high", channel = 2, weight = 30},
		["climb low"] = {animation = "climb-low", channel = 2, weight = 30},
		["hang"] = {animation = "hang", channel = 2, weight = 30},
		["hold axe"] = {animation = "hold-axe", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold blunt"] = {animation = "hold-blunt", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold bow"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold bulky"] = {animation = "hold-bulky", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold crossbow"] = {animation = "hold-crossbow", channel = 3, permanent = true, repeat_start = 10, weight = 50},
		["hold musket"] = {animation = "hold-musket", channel = 3, permanent = true, repeat_start = 10, weight = 50},
		["hold revolver"] = {animation = "hold-revolver", channel = 3, permanent = true, repeat_start = 10, weight = 50},
		["hold staff"] = {animation = "hold-staff", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold shield"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5},
		["hold sword"] = {animation = "hold-blunt", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold torch"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5},
		["dead"] = {animation = "death", channel = 1, permanent = true, repeat_start = 10, time = 10},
		["death"] = {animation = "death", channel = 1, permanent = true, repeat_start = 10},
		["drop"] = {animation = "pickup", channel = 2, weight = 10},
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
	effect_physical_damage = "fleshimpact1",
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
	feat_effects = {"berserk", "black haze", "dig", "fire damage", "firewall", "light", "physical damage", "restore health", "sanctuary", "travel", "wilderness"},
	inventory_size = 20,
	skill_quota = 200,
	speed_walk = 2,
	speed_run = 6,
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
		{"Drill", "hair4"},
		{"Short", "hair5"}},
	skin_styles = {
		{"Default", ""}}}

Species{
	name = "aer",
	base = "race",
	animations = {
		["land ground"] = {animation = "land", channel = 2, fade_in = 0.1, fade_out = 0.5, weight = 10},
		["spell self"] = {animation = "spell-self", channel = 2, weight = 30}},
	effect_falling_damage = "aerhurt1",
	equipment_class = "aer",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarm1",
		eyes = "aereye1",
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
	animations = {
		["attack axe"] = {animation = "punch", channel = 2, fade_in = 0.1, weight = 100},
		["attack blunt"] = {animation = "punch", channel = 2, fade_in = 0.1, weight = 100},
		["attack sword"] = {animation = "punch", channel = 2, weight = 100},
		["attack staff"] = {animation = "punch", channel = 2, weight = 100},
		["hold axe"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold blunt"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold bulky"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold staff"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold shield"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5},
		["hold sword"] = {animation = "hold-right", channel = 3, permanent = true, repeat_start = 10, weight = 5},
		["hold torch"] = {animation = "hold-left", channel = 4, permanent = true, repeat_start = 10, weight = 5}},
	damage_from_magma = 2,
	damage_from_water = 5,
	equipment_class = "android",
	model = "android1",
	models = {
		skeleton = "android1",
		arms = "androidarm1",
		eyes = "androideye1",
		feet = "androidfoot1",
		hands = "androidhand1",
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
		arms = "devoraarm1",
		feet = "devorafoot1",
		hands = "devorahand1",
		head = "devorahead1",
		legs = "devoraleg1",
		lower = "devoralower1",
		upper = "devoraupper1"},
	skills = {
		{name = "dexterity", max = 100, val = 50},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 40, val = 20},
		{name = "perception", max = 80, val = 30},
		{name = "strength", max = 60, val = 20},
		{name = "willpower", max = 80, val = 30}},
	vulnerabilities = {fire = 2, physical = 0.8}}

Species{
	name = "kraken",
	base = "race",
	damage_from_magma = 9,
	damage_from_water = -2,
	equipment_class = "kraken",
	model = "kraken1",
	models = {
		skeleton = "kraken1",
		arms = "krakenarm1",
		eyes = "krakeneye1",
		head = "krakenhead1",
		hands = "krakenhand1",
		feet = "krakenfoot1",
		legs = "krakenleg1",
		lower = "krakenlower1",
		upper = "krakenupper1"},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 60, val = 20},
		{name = "perception", max = 40, val = 20},
		{name = "strength", max = 80, val = 30},
		{name = "willpower", max = 100, val = 50}},
	vulnerabilities = {cold = 0.5, fire = 2},
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
		eyes = "aereye1",
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
		{name = "willpower", max = 60, val = 20}},
	vulnerabilities = {cold = 2, fire = 0.5}}

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
	inventory_items = {"torch", "workbench", "dress"}}

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
	name = "Lips guard archer",
	base = "aer",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron greaves", "iron breastplate", "iron gauntlets", "crimson crossbow", ["arrow"] = 100},
	skills = {
		{name = "dexterity", max = 80, val = 80},
		{name = "health", max = 100, val = 70},
		{name = "intelligence", max = 100, val = 0},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 20},
		{name = "willpower", max = 80, val = 0}}}

Species{
	name = "Lips guard knight",
	base = "aer",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron greaves", "iron breastplate", "iron gauntlets", "crimson sword", "round shield"},
	skills = {
		{name = "dexterity", max = 80, val = 60},
		{name = "health", max = 100, val = 70},
		{name = "intelligence", max = 100, val = 0},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 40},
		{name = "willpower", max = 80, val = 0}}}

Species{
	name = "Lips guard mage",
	base = "kraken",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"lizard dress", "lizard gloves", ["milky powder"] = 100},
	skills = {
		{name = "dexterity", max = 80, val = 5},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 60, val = 40},
		{name = "perception", max = 40, val = 20},
		{name = "strength", max = 80, val = 5},
		{name = "willpower", max = 100, val = 80}}}

Species{
	name = "lipscitizen",
	base = "aer",
	dialog = "lips citizen",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather top", "leather pants", "leather leggings"}}

Species{
	name = "lipscitizenmale",
	base = "aermale",
	dialog = "lips citizen",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather chest", "leather pants", "leather boots", "leather gloves", "leather helm"}}

Species{
	name = "seirei",
	model = "seirei1",
	ai_enable_combat = false,
	ai_enable_wander = false,
	factions = {"neutral", "seirei"}}
