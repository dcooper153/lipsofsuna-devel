Program:add_path(Mod.path)

Actorspec{
	name = "fire elemental",
	categories = {["enemy"] = true},
	ai_combat_actions = {["move forward"] = true, ["ranged spell"] = true, ["touch spell"] = true},
	animations = {--FIXME
		["attack back"] = "lizardman attack punch",
		["attack bite"] = "lizardman attack bite",
		["attack front"] = "lizardman attack punch",
		["attack left"] = "lizardman attack punch",
		["attack right"] = "lizardman attack punch",
		["attack punch"] = "lizardman attack punch",
		["dead"] = "lizardman dead",
		["death"] = "lizardman death",
		["idle"] = "lizardman idle",
		["run"] = "lizardman run",
		["run left"] = "lizardman run",
		["run right"] = "lizardman run",
		["spell ranged"] = "lizardman attack punch",
		["spell touch"] = "lizardman attack punch",
		["strafe left"] = "lizardman strafe left",
		["strafe right"] = "lizardman strafe right",
		["walk"] = "lizardman walk",
		["walk back"] = "lizardman walk back"},
	difficulty = 0.5,
	effect_physical_damage = "fleshimpact1",
	feat_types = {["ranged spell"] = true, ["spell on touch"] = true},
	feat_effects = {["fire damage"] = true},
	mass = 100,
	model = "lizardman2", --FIXME
	reagentless_spells = true,
	skills = {["Health lv2"] = true, ["Willpower lv4"] = true},
	speed_walk = 10}
