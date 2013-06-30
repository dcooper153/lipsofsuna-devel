Program:add_path(Mod.path)

Animationspec{
	name = "lizardman attack bite",
	animation = "lizardman2-bite",
	channel = 2,
	weight = 30}

Animationspec{
	name = "lizardman attack claw left",
	animation = "lizardman2-claw-left",
	channel = 2,
	weight = 30}

Animationspec{
	name = "lizardman attack claw right",
	animation = "lizardman2-claw-right",
	channel = 2,
	weight = 30}

Animationspec{
	name = "lizardman attack punch",
	animation = "lizardman2-punch",
	channel = 2,
	weight = 30}

Animationspec{
	name = "lizardman dead",
	animation = "lizardman2-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

Animationspec{
	name = "lizardman death",
	animation = "lizardman2-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "lizardman idle",
	animation = "lizardman2-idle",
	channel = 1,
	permanent = true}

Animationspec{
	name = "lizardman run",
	animation = "lizardman2-run",
	channel = 1,
	permanent = true}

Animationspec{
	name = "lizardman strafe left",
	animation = "lizardman2-strafe-left",
	channel = 1,
	permanent = true}

Animationspec{
	name = "lizardman strafe right",
	animation = "lizardman2-strafe-right",
	channel = 1,
	permanent = true}

Animationspec{
	name = "lizardman walk",
	animation = "lizardman2-walk",
	channel = 1,
	permanent = true}

Animationspec{
	name = "lizardman walk back",
	animation = "lizardman2-walk-back",
	channel = 1,
	permanent = true}

AnimationProfileSpec{
	name = "lizardman",
	animations = {
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
		["strafe left"] = "lizardman strafe left",
		["strafe right"] = "lizardman strafe right",
		["walk"] = "lizardman walk",
		["walk back"] = "lizardman walk back"}}

Itemspec{
	name = "lizard scale",
	icon = "bone1", -- FIXME
	model = "book-000", -- FIXME
	mass = 1,
	categories = {["material"] = true},
	stacking = true}

Actorspec{
	name = "lizardman",
	categories = {["enemy"] = true},
	ai_enable_block = false,
	ai_offense_factor = 0.9,
	actions = {
		["right"] = "melee",
		["left"] = "bite"},
	animations = {["default"] = "lizardman"},
	difficulty = 0.3,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	inventory_size = 10,
	inventory_items = {["wooden club"] = 1},
	inventory_items_death = {["lizard scale"] = 3, ["raw meat"] = 3},
	loot_categories = {"material"},
	loot_count_min = 2,
	loot_count_max = 4,
	mass = 100,
	model = "lizardman2",
	skills = {["Nerf health lv1"] = true},
	speed_walk = 3,
	equipment_slots = {["hand.L"] = "#hand.L", ["hand.R"] = "#hand.R"}}
