local AnimationProfileSpec = require("core/specs/animation-profile")
local AnimationSpec = require("core/specs/animation")

local frames = function(n) return n * 0.02 end

AnimationSpec{
	name = "default attack back",
	animation = "aer1-attack-back",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default attack front",
	animation = "aer1-attack-front",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default attack left",
	animation = "aer1-attack-left",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default attack right",
	animation = "aer1-attack-right",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default attack punch",
	animation = "aer1-attack-punch",
	channel = 2,
	fade_out = 0.1,
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default attack stand",
	animation = "aer1-attack-stand",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default block start",
	animation = "aer1-block",
	channel = 2,
	fade_in = 0.6,
	fade_out = 0.6,
	permanent = true,
	repeat_start = 10,
	weight = 50,
	node_weights = {LOWER = 10}}

AnimationSpec{
	name = "default block stop",
	animation = "aer1-block-stop",
	channel = 2}

AnimationSpec{
	name = "default build",
	animation = "aer1-punch",
	channel = 2,
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default charge back",
	animation = "aer1-attack-back",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	priority = 3,
	node_priorities = {ARMS = 10}}

AnimationSpec{
	name = "default charge front",
	animation = "aer1-attack-front",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	priority = 3,
	node_priorities = {ARMS = 10}}

AnimationSpec{
	name = "default charge left",
	animation = "aer1-attack-left",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	priority = 3,
	node_priorities = {ARMS = 10}}

AnimationSpec{
	name = "default charge right",
	animation = "aer1-attack-right",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	priority = 3,
	node_priorities = {ARMS = 10}}

AnimationSpec{
	name = "default charge stand",
	animation = "aer1-attack-stand",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	priority = 3,
	node_priorities = {ARMS = 10}}

AnimationSpec{
	name = "default charge cancel",
	channel = 2}

AnimationSpec{
	name = "default charge grenade",
	animation = "aer1-throw",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(20),
	repeat_start = frames(20),
	priority = 3,
	node_priorities = {ARMS = 10}}

AnimationSpec{
	name = "default charge punch",
	animation = "aer1-charge-punch",
	channel = 2,
	fade_out = 0.1,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	priority = 3,
	node_priorities = {ARMS = 10}}

AnimationSpec{
	name = "default climb high",
	animation = "aer1-climb-high",
	channel = 5,
	weight = 30}

AnimationSpec{
	name = "default climb low",
	animation = "aer1-climb-low",
	channel = 5,
	weight = 30}

AnimationSpec{
	name = "default hang",
	animation = "aer1-hang",
	channel = 2,
	weight = 30}

AnimationSpec{
	name = "default hold axe",
	animation = "aer1-hold-axe",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	priority = 15}

AnimationSpec{
	name = "default hold blunt",
	animation = "aer1-hold-blunt",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	priority = 15}

AnimationSpec{
	name = "default hold bulky",
	animation = "aer1-hold-bulky",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	priority = 15}

AnimationSpec{
	name = "default hold staff",
	animation = "aer1-hold-staff",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	priority = 15}

AnimationSpec{
	name = "default hold shield",
	animation = "aer1-hold-left",
	channel = 4,
	permanent = true,
	repeat_start = 10,
	priority = 15}

AnimationSpec{
	name = "default hold sword",
	animation = "aer1-hold-right",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	priority = 15}

AnimationSpec{
	name = "default hold torch",
	animation = "aer1-hold-left",
	channel = 4,
	permanent = true,
	repeat_start = 10,
	priority = 1}

AnimationSpec{
	name = "default dead",
	animation = "aer1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

AnimationSpec{
	name = "default death",
	animation = "aer1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

AnimationSpec{
	name = "default drop",
	animation = "aer1-pickup",
	channel = 2,
	priority = 7}

AnimationSpec{
	name = "default flinch",
	animation = "aer1-flinch1",
	animations = {"aer1-flinch1", "aer1-flinch2"},
	fade_in = 0.1,
	fade_out = 0.1,
	weight = 1000}

AnimationSpec{
	name = "default idle",
	animation = "aer1-idle2",
	channel = 1,
	priority = 1,
	weight_scale = 1,
	permanent = true}

AnimationSpec{
	name = "feminine idle",
	animation = "aer1-idle2",
	channel = 1,
	priority = 1,
	weight_scale = 1,
	permanent = true}

AnimationSpec{
	name = "default jump",
	animation = "aer1-jump",
	channel = 5,
	fade_in = 0,
	fade_out = 0.4,
	permanent = true,
	repeat_start = 50,
	priority = 7}

AnimationSpec{
	name = "default land ground",
	animation = "aer1-land",
	channel = 5,
	fade_in = 0.1,
	fade_out = 0.5,
	weight = 10,
	node_weights = {LOWER = 1000}}

AnimationSpec{
	name = "default land water",
	animation = "aer1-land",
	channel = 5,
	node_weights = {LOWER = 1000}}

AnimationSpec{
	name = "default pick up",
	animation = "aer1-pickup",
	channel = 2,
	priority = 7}

AnimationSpec{
	name = "default run",
	animation = "aer1-run",
	channel = 1,
	permanent = true,
	priority = 5}

AnimationSpec{
	name = "default run left",
	animation = "aer1-run-left",
	channel = 1,
	permanent = true,
	priority = 5}

AnimationSpec{
	name = "default run right",
	animation = "aer1-run-right",
	channel = 1,
	permanent = true,
	priority = 5}

AnimationSpec{
	name = "default spell ranged",
	animation = "aer1-cast-spell",
	channel = 2,
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default spell self",
	animation = "aer1-spell-self",
	channel = 2,
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default spell touch",
	animation = "aer1-cast-spell",
	channel = 2,
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default stagger",
	animation = "aer1-stagger-2h", --FIXME
	priority = 15}

AnimationSpec{
	name = "default strafe left",
	animation = "aer1-strafe-left",
	channel = 1,
	permanent = true,
	priority = 5}

AnimationSpec{
	name = "default strafe right",
	animation = "aer1-strafe-right",
	channel = 1,
	permanent = true,
	priority = 5}

AnimationSpec{
	name = "default talk",
	animation = "aer1-talk1",
	animations = {"aer1-talk1", "aer1-talk2", "aer1-talk3", "aer1-talk4", "aer1-talk5"},
	priority = 3}

AnimationSpec{
	name = "default throw",
	animation = "aer1-throw",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	priority = 3,
	node_priorities = {ARMS = 10}}

AnimationSpec{
	name = "default walk",
	animation = "aer1-walk",
	channel = 1,
	permanent = true,
	priority = 5}

AnimationSpec{
	name = "default walk back",
	animation = "aer1-walk-back",
	channel = 1,
	permanent = true,
	priority = 5}

AnimationProfileSpec{
	name = "default",
	animations = {
		["attack back"] = "default attack back",
		["attack front"] = "default attack front",
		["attack left"] = "default attack left",
		["attack right"] = "default attack right",
		["attack stand"] = "default attack stand",
		["attack punch"] = "default attack punch",
		["block start"] = "default block start",
		["block stop"] = "default block stop",
		["build"] = "default build",
		["charge back"] = "default charge back",
		["charge cancel"] = "default charge cancel",
		["charge front"] = "default charge front",
		["charge left"] = "default charge left",
		["charge right"] = "default charge right",
		["charge stand"] = "default charge stand",
		["charge grenade"] = "default charge grenade",
		["charge punch"] = "default charge punch",
		["climb high"] = "default climb high",
		["climb low"] = "default climb low",
		["flinch"] = "default flinch",
		["hang"] = "default hang",
		["hold"] = "default hold sword",
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
		["stagger"] = "default stagger",
		["strafe left"] = "default strafe left",
		["strafe right"] = "default strafe right",
		["talk"] = "default talk",
		["throw"] = "default throw",
		["walk"] = "default walk",
		["walk back"] = "default walk back"}}

------------------------------------------------------------------------------

AnimationSpec{
	name = "feminine idle",
	animation = "aer1-idle2",
	channel = 1,
	permanent = true}

AnimationProfileSpec{
	name = "feminine",
	animations = {
		["idle"] = "feminine idle"},
	inherit = {"default"}}

------------------------------------------------------------------------------

AnimationSpec{
	name = "default attack bow",
	animation = "aer1-attack-bow",
	channel = 2,
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default charge bow",
	animation = "aer1-charge-bow",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 100,
	node_weights = {LOWER = 1}}

AnimationSpec{
	name = "default hold bow",
	animation = "aer1-hold-right",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 5}

AnimationProfileSpec{
	name = "default bow equipped",
	animations = {
		["attack back"] = "default attack bow",
		["attack front"] = "default attack bow",
		["attack left"] = "default attack bow",
		["attack right"] = "default attack bow",
		["attack stand"] = "default attack bow",
		["charge back"] = "default charge bow",
		["charge front"] = "default charge bow",
		["charge left"] = "default charge bow",
		["charge right"] = "default charge bow",
		["charge stand"] = "default charge bow"}}
		["hold"] = "default hold bow",

------------------------------------------------------------------------------

AnimationSpec{
	name = "default attack crossbow",
	animation = "aer1-attack-crossbow",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default charge crossbow",
	animation = "aer1-attack-crossbow",
	channel = 2,
	fade_in = 0.1,
	permanent = true,
	repeat_end = frames(50),
	repeat_start = frames(50),
	weight = 5000,
	node_weights = {LOWER = 1}}

AnimationSpec{
	name = "default hold crossbow",
	animation = "aer1-hold-crossbow",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 50}

AnimationProfileSpec{
	name = "default crossbow equipped",
	animations = {
		["attack back"] = "default attack crossbow",
		["attack front"] = "default attack crossbow",
		["attack left"] = "default attack crossbow",
		["attack right"] = "default attack crossbow",
		["attack stand"] = "default attack crossbow",
		["charge back"] = "default charge crossbow",
		["charge front"] = "default charge crossbow",
		["charge left"] = "default charge crossbow",
		["charge right"] = "default charge crossbow",
		["charge stand"] = "default charge crossbow",
		["hold"] = "default hold crossbow"}}

------------------------------------------------------------------------------

AnimationSpec{
	name = "default attack musket",
	animation = "aer1-attack-musket",
	channel = 2,
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default charge musket",
	animation = "aer1-charge-musket",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 5000,
	node_weights = {LOWER = 1}}

AnimationSpec{
	name = "default hold musket",
	animation = "aer1-hold-musket",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 50}

AnimationProfileSpec{
	name = "default musket equipped",
	animations = {
		["attack back"] = "default attack musket",
		["attack front"] = "default attack musket",
		["attack left"] = "default attack musket",
		["attack right"] = "default attack musket",
		["attack stand"] = "default attack musket",
		["charge back"] = "default charge musket",
		["charge front"] = "default charge musket",
		["charge left"] = "default charge musket",
		["charge right"] = "default charge musket",
		["charge stand"] = "default charge musket",
		["hold"] = "default hold musket"}}

------------------------------------------------------------------------------

AnimationSpec{
	name = "default attack revolver",
	animation = "aer1-attack-revolver",
	channel = 2,
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default charge revolver",
	animation = "aer1-charge-revolver",
	channel = 2,
	permanent = true,
	repeat_start = 10,
	weight = 5000,
	node_weights = {LOWER = 1}}

AnimationSpec{
	name = "default hold revolver",
	animation = "aer1-hold-revolver",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 50}

AnimationProfileSpec{
	name = "default revolver equipped",
	animations = {
		["attack back"] = "default attack revolver",
		["attack front"] = "default attack revolver",
		["attack left"] = "default attack revolver",
		["attack right"] = "default attack revolver",
		["attack stand"] = "default attack revolver",
		["charge back"] = "default charge revolver",
		["charge front"] = "default charge revolver",
		["charge left"] = "default charge revolver",
		["charge right"] = "default charge revolver",
		["charge stand"] = "default charge revolver",
		["hold"] = "default hold revolver"}}

------------------------------------------------------------------------------

AnimationSpec{
	name = "default attack back two-handed",
	animation = "aer1-attack-back-2h",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	priority = 10,
	node_priorities = {LOWER = 3}}

AnimationSpec{
	name = "default attack front two-handed",
	animation = "aer1-attack-front-2h",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	node_weights = {LOWER = 1}}

AnimationSpec{
	name = "default attack stand two-handed",
	animation = "aer1-attack-stand-2h",
	channel = 2,
	fade_in = 0,
	replace = true,
	time = frames(20),
	weight = 1000,
	node_weights = {LOWER = 1}}

AnimationSpec{
	name = "default hold two-handed",
	animation = "aer1-hold-2h",
	channel = 3,
	permanent = true,
	repeat_start = 10,
	weight = 5}

AnimationProfileSpec{
	name = "default two-handed equipped",
	animations = {
		["attack back"] = "default attack back two-handed",
		["attack front"] = "default attack front two-handed",
		["attack left"] = "default attack stand two-handed", -- TODO
		["attack right"] = "default attack stand two-handed", -- TODO
		["attack stand"] = "default attack stand two-handed",
		["hold"] = "default hold two-handed"}}

------------------------------------------------------------------------------

-- FIXME
AnimationProfileSpec{
	name = "default sword equipped"}
