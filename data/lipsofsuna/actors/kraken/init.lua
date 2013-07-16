Program:add_path(Mod.path)

AnimationProfileSpec{
	name = "kraken",
	animations = {
		["attack back"] = "kraken attack back",
		["attack bow"] = "kraken attack bow",
		["attack crossbow"] = "kraken attack crossbow",
		["attack front"] = "kraken attack front",
		["attack left"] = "kraken attack left",
		["attack right"] = "kraken attack right",
		["attack punch"] = "kraken attack punch",
		["attack stand"] = "kraken attack stand",
		["attack musket"] = "kraken attack musket",
		["attack revolver"] = "kraken attack revolver",
		["build"] = "kraken build",
		["charge axe"] = "kraken charge axe",
		["charge blunt"] = "kraken charge blunt",
		["charge bow"] = "kraken charge bow",
		["charge cancel"] = "kraken charge cancel",
		["charge crossbow"] = "kraken charge crossbow",
		["charge grenade"] = "kraken charge grenade",
		["charge musket"] = "kraken charge musket",
		["charge punch"] = "kraken charge punch",
		["charge revolver"] = "kraken charge revolver",
		["charge sword"] = "kraken charge sword",
		["charge staff"] = "kraken charge staff",
		["climb high"] = "kraken climb high",
		["climb low"] = "kraken climb low",
		["hang"] = "kraken hang",
		["dead"] = "kraken dead",
		["death"] = "kraken death",
		["drop"] = "kraken drop",
		["idle"] = "kraken idle",
		["jump"] = "kraken jump",
		["land ground"] = "kraken land ground",
		["land water"] = "kraken land water",
		["pick up"] = "kraken pick up",
		["run"] = "kraken run",
		["run left"] = "kraken run left",
		["run right"] = "kraken run right",
		["spell ranged"] = "kraken spell ranged",
		["spell self"] = "kraken spell self",
		["spell touch"] = "kraken spell touch",
		["strafe left"] = "kraken strafe left",
		["strafe right"] = "kraken strafe right",
		["throw"] = "kraken throw",
		["throw bulky"] = "kraken throw",
		["throw grenade"] = "kraken throw",
		["walk"] = "kraken walk",
		["walk back"] = "kraken walk back"}}

Actorspec{
	name = "kraken",
	base = "race",
	animations = {["default"] = "kraken"},
	damage_from_magma = 9,
	damage_from_water = -2,
	equipment_class = "kraken",
	model = "kraken1",
	models = {
		skeleton = "kraken1",
		arms = "aerarm1",
		head = "aerhead1",
		hands = "aerhand1",
		feet = "krakenfoot1",
		legs = "krakenleg1",
		lower = "krakenlower1",
		upper = "aerupper1",
		upper_safe = "aerbloomerstop1"},
	skin_material = "animskin1",
	skin_textures = {"kraken1", "kraken1n"},
	tilt_bone = {"back1", "back2", "back3"},
	vulnerabilities = {["cold damage"] = 0.5, ["fire damage"] = 2},
	water_friction = 0}

Actorspec{
	name = "kraken-player",
	base = "kraken",
	ai_enabled = false,
	inventory_items = {["torch"] = 1, ["workbench"] = 1, ["dress"] = 1}}

Actorskinspec{
	name = "Kraken skin 1",
	actors = {["kraken"] = true},
	material = "animskin1",
	textures = {"kraken1", "kraken1n"}}

Animationspec{
	name = "kraken attack back",
	animation = "aer1-attack-back",
	channel = 2,
	fade_in = 0.1,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken attack bow",
	animation = "kraken1-attack-bow",
	channel = 2,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken attack crossbow",
	animation = "kraken1-attack-crossbow",
	channel = 2,
	weight = 5000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken attack front",
	animation = "kraken1-attack-front",
	channel = 2,
	fade_in = 0.1,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken attack left",
	animation = "kraken1-attack-left",
	channel = 2,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken attack right",
	animation = "kraken1-attack-right",
	channel = 2,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken attack punch",
	animation = "kraken1-attack-punch",
	channel = 2,
	fade_out = 0.1,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken attack stand",
	animation = "kraken1-attack-stand",
	channel = 2,
	fade_in = 0.1,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken attack musket",
	animation = "kraken1-attack-musket",
	channel = 2,
	weight = 5000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken attack revolver",
	animation = "kraken1-attack-revolver",
	channel = 2,
	weight = 5000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken build",
	animation = "kraken1-punch",
	channel = 2,
	weight = 30}

Animationspec{
	name = "kraken charge axe",
	animation = "kraken1-charge-melee",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken charge blunt",
	animation = "kraken1-charge-melee",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken charge bow",
	animation = "kraken1-charge-bow",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken charge cancel",
	channel = 2}

Animationspec{
	name = "kraken charge crossbow",
	animation = "kraken1-charge-crossbow",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 5000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken charge grenade",
	animation = "kraken1-charge-throw",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken charge musket",
	animation = "kraken1-charge-musket",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 5000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken charge punch",
	animation = "kraken1-charge-punch",
	channel = 2,
	fade_out = 0.1,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1,
	ARML = 1}}

Animationspec{
	name = "kraken charge revolver",
	animation = "kraken1-charge-revolver",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 5000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken charge sword",
	animation = "kraken1-charge-melee",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken charge staff",
	animation = "kraken1-charge-melee",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "kraken climb high",
	animation = "kraken1-climb-high",
	channel = 5,
	weight = 30}

Animationspec{
	name = "kraken climb low",
	animation = "kraken1-climb-low",
	channel = 5,
	weight = 30}

Animationspec{
	name = "kraken hang",
	animation = "kraken1-hang",
	channel = 2,
	weight = 30}

Animationspec{
	name = "kraken dead",
	animation = "kraken1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

Animationspec{
	name = "kraken death",
	animation = "kraken1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "kraken drop",
	animation = "kraken1-pickup",
	channel = 2,
	weight = 10}

Animationspec{
	name = "kraken idle",
	animation = "kraken1-idle",
	channel = 1,
	permanent = true}

Animationspec{
	name = "kraken jump",
	animation = "kraken1-jump",
	channel = 5,
	fade_out = 0.4,
	permanent = true,
	repeat_start = 10,
	weight = 10,
	node_weights = {LOWER = 1000}}

Animationspec{
	name = "kraken land ground",
	animation = "kraken1-land",
	channel = 5,
	fade_in = 0.1,
	fade_out = 0.5,
	weight = 10,
	node_weights = {LOWER = 1000}}

Animationspec{
	name = "kraken land water",
	animation = "kraken1-land",
	channel = 5,
	node_weights = {LOWER = 1000}}

Animationspec{
	name = "kraken pick up",
	animation = "kraken1-pickup",
	channel = 2,
	weight = 10}

Animationspec{
	name = "kraken run",
	animation = "kraken1-run",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "kraken run left",
	animation = "kraken1-run-left",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "kraken run right",
	animation = "kraken1-run-right",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "kraken spell ranged",
	animation = "kraken1-cast-spell",
	channel = 2,
	weight = 30}

Animationspec{
	name = "kraken spell self",
	animation = "kraken1-spell-self",
	channel = 2,
	weight = 30}

Animationspec{
	name = "kraken spell touch",
	animation = "kraken1-cast-spell",
	channel = 2,
	weight = 30}

Animationspec{
	name = "kraken strafe left",
	animation = "kraken1-strafe-left",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "kraken strafe right",
	animation = "kraken1-strafe-right",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "kraken throw",
	animation = "kraken1-throw",
	channel = 2,
	weight = 10}

Animationspec{
	name = "kraken walk",
	animation = "kraken1-walk",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "kraken walk back",
	animation = "kraken1-walk-back",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}
