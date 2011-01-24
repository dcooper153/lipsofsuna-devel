local regen = 0.5

------------------------------------------------------------------------------
-- Base races.

Species{
	name = "race",
	aim_ray_center = Vector(0, 1.5, 0),
	aim_ray_end = 5,
	aim_ray_start = 0.5,
	equipment_slots = {
		{name = "arms"},
		{name = "feet"},
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"},
		{name = "head"},
		{name = "lowerbody"},
		{name = "upperbody"}},
	eye_styles = {
		{"Default", ""}},
	factions = {"good"},
	feats = {"build", "right hand", "ranged", "ranged spell", "spell on self", "spell on touch", "throw"},
	hair_styles = {
		{"Bald", ""},
		{"Long", "hair2"},
		{"Pigtails", "hair1"}},
	inventory_size = 20,
	skill_quota = 200,
	speed_walk = 2,
	speed_run = 4,
	tilt_bone = "back",
	tilt_limit = 1.25}

Species{
	name = "aer",
	base = "race",
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
	name = "android",
	base = "race",
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
		{name = "dexterity", max = 60, val = 20},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 80, val = 30},
		{name = "perception", max = 100, val = 50},
		{name = "strength", max = 80, val = 30},
		{name = "willpower", max = 40, val = 20}}}

Species{
	name = "devora",
	base = "race",
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
		{name = "dexterity", max = 100, val = 50},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 40, val = 20},
		{name = "perception", max = 80, val = 30},
		{name = "strength", max = 60, val = 20},
		{name = "willpower", max = 80, val = 30}}}

Species{
	name = "kraken",
	base = "race",
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

------------------------------------------------------------------------------
-- Playable races.

Species{
	name = "aer-player",
	base = "aer",
	ai_enabled = false,
	inventory_items = {"saw", "torch", "wooden dagger", "wooden mattock", "dress", "leather leggings"}}

Species{
	name = "devora-player",
	base = "devora",
	ai_enabled = false,
	inventory_items = {"saw", "torch", "wooden dagger", "wooden mattock", "dress", "leather leggings"}}

Species{
	name = "kraken-player",
	base = "kraken",
	ai_enabled = false,
	inventory_items = {"saw", "torch", "wooden dagger", "wooden mattock", "dress", "leather leggings"}}

Species{
	name = "wyrm-player",
	base = "wyrm",
	ai_enabled = false,
	inventory_items = {"saw", "torch", "wooden dagger", "wooden mattock", "dress", "leather leggings"}}

Species{
	name = "companion",
	base = Species.base_aer,
	dialog = "companion",
	hair_style = {"hair1",255,255,255},
	inventory_items = {"saw", "torch", "wooden dagger", "wooden mattock", "dress", "leather leggings"}}

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
	name = "bloodworm",
	ai_enable_strafe = false,
	ai_enable_defend = false,
	categories = {"enemy"},
	factions = {"evil"},
	feats = {"bite"},
	inventory_size = 0,
	model = "bloodworm1",
	speed_walk = 1,
	skills = {
		{name = "health", max = 30, val = 30}}}

Species{
	name = "dragon",
	model = "dragon1",
	ai_distance_hint = 2.5,
	ai_enable_strafe = false,
	ai_enable_defend = false,
	factions = {"evil"},
	feats = {"dragon bite", "left claw", "right claw"},
	inventory_size = 0,
	mass = 1000,
	speed_walk = 1,
	skills = {
		{name = "health", max = 100, val = 100}}}
