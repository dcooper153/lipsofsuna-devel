require(Mod.path .. "feats")

Actorspec{
	name = "race",
	aim_ray_center = Vector(0, 1.5, 0),
	aim_ray_end = 5,
	aim_ray_start = 0.5,
	animations = {["default"] = "default", ["feminine"] = "feminine"},
	camera_center = Vector(0, 1.5, 0),
	effect_physical_damage = "fleshimpact1",
	equipment_slots = {
		["arms"] = "",
		["feet"] = "",
		["hand.L"] = "#hand.L",
		["hand.R"] = "#hand.R",
		["head"] = "",
		["lowerbody"] = "",
		["upperbody"] = ""},
	factions = {["good"] = true},
	feat_types = {["build"] = true, ["right hand"] = true, ["ranged"] = true, ["throw"] = true},
	footstep_height = 0.3,
	footstep_sound = "step1",
	inventory_size = 20,
	skill_quota = 200,
	speed_walk = 3.5,
	speed_run = 10.5,
	tilt_bone = {"backtilt"},
	tilt_limit = 1.25,
	timing_build = 0.2,
	timing_jump = 0,
	timing_pickup = 0.2,
	-- Customization.
	body_scale_min = 0.9,
	body_scale_max = 1.05,
	eye_styles = {["Default"] = "aereye1", ["Angled"] = "aereye2"},
	hair_styles = {["Bald"] = "", ["Pigtails"] = "hair1", ["Pigtails 2"] = "hair6", ["Pigtails 3"] = "hair3", ["Long"] = "hair2", ["Drill"] = "hair4", ["Short"] = "hair5"},
		--{"Short", "aershorthair1"} -- TODO: Adapt to head mesh changes
	head_styles = {["Head 1"] = "aerhead1"},
	skin_styles = {["Default"] = ""}}
