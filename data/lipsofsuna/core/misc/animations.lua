local frames = function(n) return n * 0.02 end

Animationspec{
	name = "default attack back",
	animation = "aer1-attack-back",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default attack bow",
	animation = "aer1-attack-bow",
	channel = 2,
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default attack crossbow",
	animation = "aer1-attack-crossbow",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default attack front",
	animation = "aer1-attack-front",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default attack left",
	animation = "aer1-attack-left",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default attack right",
	animation = "aer1-attack-right",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default attack punch",
	animation = "aer1-attack-punch",
	channel = 2,
	fade_out = 0.1,
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default attack stand",
	animation = "aer1-attack-stand",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default attack musket",
	animation = "aer1-attack-musket",
	channel = 2,
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default attack revolver",
	animation = "aer1-attack-revolver",
	channel = 2,
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default block start",
	animation = "aer1-block",
	channel = 2,
	fade_in = 0.6,
	fade_out = 0.6,
	permanent = true,
	repeat_start = 10,
	weight = 50}

Animationspec{
	name = "default block stop",
	animation = "aer1-block-stop",
	channel = 2}

Animationspec{
	name = "default build",
	animation = "aer1-punch",
	channel = 2,
	weight = 30}

Animationspec{
	name = "default charge back",
	animation = "aer1-attack-back",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default charge front",
	animation = "aer1-attack-front",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default charge left",
	animation = "aer1-attack-left",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default charge right",
	animation = "aer1-attack-right",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default charge stand",
	animation = "aer1-attack-stand",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default charge bow",
	animation = "aer1-charge-bow",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default charge cancel",
	channel = 2}

Animationspec{
	name = "default charge crossbow",
	animation = "aer1-attack-crossbow",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(50),
	repeat_start = frames(50),
	weight = 5000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default charge grenade",
	animation = "aer1-throw",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	weight = 100,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default charge musket",
	animation = "aer1-charge-musket",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 5000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default charge punch",
	animation = "aer1-charge-punch",
	channel = 2,
	fade_out = 0.1,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1,
	ARML = 1}}

Animationspec{
	name = "default charge revolver",
	animation = "aer1-charge-revolver",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 5000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default climb high",
	animation = "aer1-climb-high",
	channel = 5,
	weight = 30}

Animationspec{
	name = "default climb low",
	animation = "aer1-climb-low",
	channel = 5,
	weight = 30}

Animationspec{
	name = "default hang",
	animation = "aer1-hang",
	channel = 2,
	weight = 30}

Animationspec{
	name = "default hold axe",
	animation = "aer1-hold-axe",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 5}

Animationspec{
	name = "default hold blunt",
	animation = "aer1-hold-blunt",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 5}

Animationspec{
	name = "default hold bow",
	animation = "aer1-hold-right",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 5}

Animationspec{
	name = "default hold bulky",
	animation = "aer1-hold-bulky",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 5}

Animationspec{
	name = "default hold crossbow",
	animation = "aer1-hold-crossbow",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 50}

Animationspec{
	name = "default hold musket",
	animation = "aer1-hold-musket",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 50}

Animationspec{
	name = "default hold revolver",
	animation = "aer1-hold-revolver",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 50}

Animationspec{
	name = "default hold staff",
	animation = "aer1-hold-staff",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 5}

Animationspec{
	name = "default hold shield",
	animation = "aer1-hold-left",
	channel = 4,
	permanent = true,
	repeat_start = 10,
	weight = 5}

Animationspec{
	name = "default hold sword",
	animation = "aer1-hold-right",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 5}

Animationspec{
	name = "default hold torch",
	animation = "aer1-hold-left",
	channel = 4,
	permanent = true,
	repeat_start = 10,
	weight = 5}

Animationspec{
	name = "default dead",
	animation = "aer1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

Animationspec{
	name = "default death",
	animation = "aer1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "default drop",
	animation = "aer1-pickup",
	channel = 2,
	weight = 10}

Animationspec{
	name = "default idle",
	animation = "aer1-idle",
	channel = 1,
	permanent = true}

Animationspec{
	name = "feminine idle",
	animation = "aer1-idle2",
	channel = 1,
	permanent = true}

Animationspec{
	name = "default jump",
	animation = "aer1-jump",
	channel = 5,
	fade_out = 0.4,
	permanent = true,
	repeat_start = 10,
	weight = 10,
	node_weights = {LOWER = 1000}}

Animationspec{
	name = "default land ground",
	animation = "aer1-land",
	channel = 5,
	fade_in = 0.1,
	fade_out = 0.5,
	weight = 10,
	node_weights = {LOWER = 1000}}

Animationspec{
	name = "default land water",
	animation = "aer1-land",
	channel = 5,
	node_weights = {LOWER = 1000}}

Animationspec{
	name = "default pick up",
	animation = "aer1-pickup",
	channel = 2,
	weight = 10}

Animationspec{
	name = "default run",
	animation = "aer1-run",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "default run left",
	animation = "aer1-run-left",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "default run right",
	animation = "aer1-run-right",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "default spell ranged",
	animation = "aer1-cast-spell",
	channel = 2,
	weight = 30}

Animationspec{
	name = "default spell self",
	animation = "aer1-spell-self",
	channel = 2,
	weight = 30}

Animationspec{
	name = "default spell touch",
	animation = "aer1-cast-spell",
	channel = 2,
	weight = 30}

Animationspec{
	name = "default strafe left",
	animation = "aer1-strafe-left",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "default strafe right",
	animation = "aer1-strafe-right",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "default throw",
	animation = "aer1-throw",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	node_weights = {LOWER = 1}}

Animationspec{
	name = "default walk",
	animation = "aer1-walk",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

Animationspec{
	name = "default walk back",
	animation = "aer1-walk-back",
	channel = 1,
	permanent = true,
	node_weights = {LOWER = 100}}

AnimationProfileSpec{
	name = "default",
	animations = {
		["attack back"] = "default attack back",
		["attack front"] = "default attack front",
		["attack left"] = "default attack left",
		["attack right"] = "default attack right",
		["attack stand"] = "default attack stand",
		["attack bow"] = "default attack bow",
		["attack crossbow"] = "default attack crossbow",
		["attack musket"] = "default attack musket",
		["attack punch"] = "default attack punch",
		["attack revolver"] = "default attack revolver",
		["block start"] = "default block start",
		["block stop"] = "default block stop",
		["build"] = "default build",
		["charge back"] = "default charge back",
		["charge cancel"] = "default charge cancel",
		["charge front"] = "default charge front",
		["charge left"] = "default charge left",
		["charge right"] = "default charge right",
		["charge stand"] = "default charge stand",
		["charge bow"] = "default charge bow",
		["charge crossbow"] = "default charge crossbow",
		["charge grenade"] = "default charge grenade",
		["charge musket"] = "default charge musket",
		["charge punch"] = "default charge punch",
		["charge revolver"] = "default charge revolver",
		["climb high"] = "default climb high",
		["climb low"] = "default climb low",
		["hang"] = "default hang",
		["hold axe"] = "default hold axe",
		["hold blunt"] = "default hold blunt",
		["hold bow"] = "default hold bow",
		["hold bulky"] = "default hold bulky",
		["hold crossbow"] = "default hold crossbow",
		["hold musket"] = "default hold musket",
		["hold revolver"] = "default hold revolver",
		["hold staff"] = "default hold staff",
		["hold shield"] = "default hold shield",
		["hold sword"] = "default hold sword",
		["hold torch"] = "default hold torch",
		["dead"] = "default dead",
		["death"] = "default death",
		["drop"] = "default drop",
		["idle"] = "default idle",
		["jump"] = "default jump",
		["land ground"] = "default land ground",
		["land water"] = "default land water",
		["pick up"] = "default pick up",
		["run"] = "default run",
		["run left"] = "default run left",
		["run right"] = "default run right",
		["spell ranged"] = "default spell ranged",
		["spell self"] = "default spell self",
		["spell touch"] = "default spell touch",
		["strafe left"] = "default strafe left",
		["strafe right"] = "default strafe right",
		["throw"] = "default throw",
		["throw bulky"] = "default throw",
		["throw grenade"] = "default throw",
		["walk"] = "default walk",
		["walk back"] = "default walk back"}}

Animationspec{
	name = "feminine idle",
	animation = "aer1-idle2",
	channel = 1,
	permanent = true}

AnimationProfileSpec{
	name = "feminine",
	animations = {
		["idle"] = "feminine idle"},
	inherit = {"default"}}
