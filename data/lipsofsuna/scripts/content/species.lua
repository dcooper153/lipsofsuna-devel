local regen = 0.5

------------------------------------------------------------------------------
-- Base races.

Species{
	name = "race",
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
		{"Long", "hair2"},
		{"Pigtails", "hair1"},
		{"Short", "aershorthair1"}},
	skin_styles = {
		{"Default", ""}}}

Species{
	name = "aer",
	base = "race",
	equipment_class = "aer",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarmnude1",
		head = "aerhead1",
		legs = "aerlegnude1",
		lower = "aerlowernude1",
		upper = "aeruppernude1"},
	models_lod = {
		skeleton = "aer1",
		arms = "aerarmnude1l",
		head = "aerhead1l",
		legs = "aerlegnude1l",
		lower = "aerlowernude1l",
		upper = "aeruppernude1l"},
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
		{"Short", "aermale1hair1"}}}

Species{
	name = "android",
	base = "race",
	equipment_class = "android",
	model = "android1",
	models = {
		skeleton = "android1",
		arms = "androidarmnude1",
		feet = "androidfootnude1",
		hand = "androidhand1",
		head = "androidhead1",
		legs = "androidlegnude1",
		lower = "androidlowernude1",
		upper = "androiduppernude1"},
	skills = {
		{name = "dexterity", max = 60, val = 20},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 80, val = 30},
		{name = "perception", max = 100, val = 50},
		{name = "strength", max = 80, val = 30},
		{name = "willpower", max = 40, val = 20}},
	hair_styles = {
		{"Bald", ""},
		{"Pigtails", "androidhair1"},
		{"Long", "androidhair2"},
		{"Drill", "androidhair3"}}}

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
	equipment_class = "kraken",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarmnude1",
		head = "aerhead1",
		legs = "aerlegnude1",
		lower = "aerlowernude1",
		upper = "aeruppernude1"},
	models_lod = {
		skeleton = "aer1",
		arms = "aerarmnude1l",
		head = "aerhead1l",
		legs = "aerlegnude1l",
		lower = "aerlowernude1l",
		upper = "aeruppernude1l"},
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 60, val = 20},
		{name = "perception", max = 40, val = 20},
		{name = "strength", max = 80, val = 30},
		{name = "willpower", max = 100, val = 50}}}

Species{
	name = "wyrm",
	base = "race",
	equipment_class = "wyrm",
	model = "aer1",
	models = {
		skeleton = "aer1",
		arms = "aerarmnude1",
		head = "aerhead1",
		legs = "aerlegnude1",
		lower = "aerlowernude1",
		upper = "aeruppernude1"},
	models_lod = {
		skeleton = "aer1",
		arms = "aerarmnude1l",
		head = "aerhead1l",
		legs = "aerlegnude1l",
		lower = "aerlowernude1l",
		upper = "aeruppernude1l"},
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
	inventory_items = {"iron sword", "leather top", "leather pants", "leather leggings"}}

Species{
	name = "brigandmale",
	base = "aermale",
	categories = {"enemy"},
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather brigand chest", "leather brigand pants", "leather boots", "leather gloves", "leather helm"}}

Species{
	name = "troglodyte female",
	base = "trogfemale",
	categories = {"enemy"},
	factions = {"evil"},
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"wooden club", "trog rags"}}

Species{
	name = "bloodworm",
	ai_enable_strafe = false,
	ai_enable_defend = false,
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
	model = "dragon1",
	ai_distance_hint = 2.5,
	ai_enable_strafe = false,
	ai_enable_defend = false,
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
	inventory_items = {"dress"}}

Species{
	name = "lizardman",
	model = "lizardman2",
	ai_distance_hint = 1.0,
	ai_enable_defend = false,
	categories = {"enemy"},
	factions = {"evil"},
	feat_anims = {"right hand","bite"},
	inventory_size = 5,
	inventory_items = {"wooden club", "lizard scale", "lizard scale", "lizard scale"},
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
	ai_enable_strafe = false,
	ai_enable_defend = false,
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
	ai_enable_strafe = false,
	ai_enable_defend = false,
	categories = {"enemy", "plague"},
	factions = {"evil"},
	feat_anims = {"bite"},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2", -- FIXME
	speed_walk = 2,
	skills = {
		{name = "health", max = 30, val = 30}}}

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
