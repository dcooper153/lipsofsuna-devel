Program:add_path(Mod.path)

Animationspec{
	name = "bitingbark attack claw left",
	animation = "bitingbark1-claw-left",
	channel = 2,
	weight = 30}

Animationspec{
	name = "bitingbark attack claw right",
	animation = "bitingbark1-claw-right",
	channel = 2,
	weight = 30}

Animationspec{
	name = "bitingbark dead",
	animation = "bitingbark1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

Animationspec{
	name = "bitingbark death",
	animation = "bitingbark1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "bitingbark idle",
	animation = "bitingbark1-idle",
	channel = 1,
	permanent = true}

Animationspec{
	name = "bitingbark run",
	animation = "bitingbark1-run",
	channel = 1,
	permanent = true}

Animationspec{
	name = "bitingbark walk",
	animation = "bitingbark1-walk",
	channel = 1,
	permanent = true}

Animationspec{
	name = "bitingbark walk back",
	animation = "bitingbark1-walk-back",
	channel = 1,
	permanent = true}

Actorspec{
	name = "bitingbark",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_enable_jump = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 1,
	animations = {
		["attack left claw"] = "bitingbark attack claw left",
		["attack right claw"] = "bitingbark attack claw right",
		["dead"] = "bitingbark dead",
		["death"] = "bitingbark death",
		["idle"] = "bitingbark idle",
		["run"] = "bitingbark run",
		["run left"] = "bitingbark run",
		["run right"] = "bitingbark run",
		["strafe left"] = "bitingbark walk",
		["strafe right"] = "bitingbark walk",
		["walk"] = "bitingbark walk",
		["walk back"] = "bitingbark walk back"},
	categories = {["enemy"] = true},
	difficulty = 0.3,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	feat_types = {["left claw"] = true, ["right claw"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 3,
	inventory_items = {["log"] = 2},
	jump_force = 0,
	model = "bitingbark1",
	skills = {["Nerf combat lv1"] = true},
	speed_walk = 1}
