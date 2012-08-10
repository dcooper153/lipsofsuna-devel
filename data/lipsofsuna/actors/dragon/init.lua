Program:add_path(Mod.path)

Animationspec{
	name = "dragon attack bite",
	animation = "dragon1-bite",
	channel = 2,
	weight = 30}

Animationspec{
	name = "dragon attack left claw",
	animation = "dragon1-claw-left",
	channel = 2,
	weight = 30}

Animationspec{
	name = "dragon attack right claw",
	animation = "dragon1-claw-right",
	channel = 2,
	weight = 30}

Animationspec{
	name = "dragon dead",
	animation = "dragon1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

Animationspec{
	name = "dragon death",
	animation = "dragon1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "dragon idle",
	animation = "dragon1-idle",
	channel = 1,
	permanent = true}

Animationspec{
	name = "dragon spell ranged",
	animation = "breathe-fire",
	channel = 2,
	weight = 30}

Animationspec{
	name = "dragon walk",
	animation = "dragon1-walk",
	channel = 1,
	permanent = true}

Animationspec{
	name = "dragon walk back",
	animation = "dragon1-walk-back",
	channel = 1,
	permanent = true}

AnimationProfileSpec{
	name = "dragon",
	animations = {
		["attack bite"] = "dragon attack bite",
		["attack left claw"] = "dragon attack left claw",
		["attack right claw"] = "dragon attack right claw",
		["dead"] = "dragon dead",
		["death"] = "dragon death",
		["idle"] = "dragon idle",
		["run"] = "dragon walk",
		["run left"] = "dragon walk",
		["run right"] = "dragon walk",
		["spell ranged"] = "dragon spell ranged",
		["strafe left"] = "dragon walk",
		["strafe right"] = "dragon walk",
		["walk"] = "dragon walk",
		["walk back"] = "dragon walk back"}}

Effectspec{
	name = "dragonbreath1",
	node = "head",
	particle = "firebreath1",
	sound = "squish-000"} --FIXME

Itemspec{
	name = "dragon scale",
	icon = "bone1", -- FIXME
	model = "book-000", -- FIXME
	mass = 1,
	categories = {["material"] = true},
	stacking = true}

Actorspec{
	name = "dragon",
	model = "dragon1",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {["default"] = "dragon"},
	difficulty = 1,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	feat_types = {["dragon bite"] = true, ["dragon left claw"] = true, ["dragon right claw"] = true},--, ["ranged spell"] = true},
	feat_effects = {["fire damage"] = true, ["physical damage"] = true},
	important = true,
	inventory_size = 10,
	inventory_items = {["dragon scale"] = 5, ["adamantium"] = 4, ["crimson stone"] = 2},
	mass = 1000,
	speed_walk = 1,
	skills = {["Health lv4"] = true, ["Willpower lv2"] = true, ["Fighter"] = true},
	vulnerabilities = {["cold damage"] = 2, ["fire damage"] = 0.3}}
