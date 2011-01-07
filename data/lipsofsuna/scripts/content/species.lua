local regen = 0.5

Species{
	name = "aer",
	equipment_slots = {
		{name = "arms"},
		{name = "feet"},
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"},
		{name = "head"},
		{name = "lowerbody"},
		{name = "upperbody"}},
	factions = {"good"},
	feats = {"build", "right hand", "ranged", "ranged spell", "spell on self", "spell on touch", "throw"},
	inventory_size = 20,
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
	skill_quota = 200,
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 100, val = 50},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 20},
		{name = "willpower", max = 80, val = 30}},
	speed_walk = 2,
	speed_run = 4,
	tilt_bone = "back",
	tilt_limit = 1.25}

Species{
	name = "aer-player",
	base = "aer",
	ai_enabled = false,
	inventory_items = {"saw", "torch", "boomerang", "wooden mattock", "dress", "leather leggings"}}

Species{
	name = "android",
	equipment_slots = {
		{name = "arms"},
		{name = "feet"},
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"},
		{name = "head"},
		{name = "lowerbody"},
		{name = "upperbody"}},
	factions = {"good"},
	feats = {"attack"},
	inventory_size = 20,
	model = "aer1",
	skill_quota = 200,
	skills = {
		{name = "dexterity", max = 60, val = 20},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 80, val = 30},
		{name = "perception", max = 100, val = 50},
		{name = "strength", max = 80, val = 30},
		{name = "willpower", max = 40, val = 20}},
	speed_walk = 3,
	speed_run = 6,
	tilt_bone = "back",
	tilt_limit = 1.25}

Species{
	name = "devora",
	equipment_slots = {
		{name = "arms"},
		{name = "feet"},
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"},
		{name = "head"},
		{name = "lowerbody"},
		{name = "upperbody"}},
	factions = {"good"},
	feats = {"attack"},
	inventory_size = 20,
	model = "aer1",
	models = { -- TODO: Models needed
		skeleton = "aer1",
		arms = "aerarmnude1",
		head = "aerhead1",
		legs = "aerlegnude1",
		lower = "aerlowernude1",
		upper = "aeruppernude1"},
	skill_quota = 200,
	skills = {
		{name = "dexterity", max = 100, val = 50},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 40, val = 20},
		{name = "perception", max = 80, val = 30},
		{name = "strength", max = 60, val = 20},
		{name = "willpower", max = 80, val = 30}},
	speed_walk = 3,
	speed_run = 6,
	tilt_bone = "back",
	tilt_limit = 1.25}

Species{
	name = "devora-player",
	base = "devora",
	ai_enabled = false,
	inventory_items = {"torch", "mattock", "chainmail tunic", "chainmail pants", "chainmail leggings"}}

Species{
	name = "kraken",
	equipment_slots = {
		{name = "arms"},
		{name = "feet"},
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"},
		{name = "head"},
		{name = "lowerbody"},
		{name = "upperbody"}},
	factions = {"good"},
	feats = {"attack"},
	inventory_size = 20,
	model = "aer1",
	models = { -- TODO: Models needed
		skeleton = "aer1",
		arms = "aerarmnude1",
		head = "aerhead1",
		legs = "aerlegnude1",
		lower = "aerlowernude1",
		upper = "aeruppernude1"},
	skill_quota = 200,
	skills = {
		{name = "dexterity", max = 80, val = 30},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 60, val = 20},
		{name = "perception", max = 40, val = 20},
		{name = "strength", max = 80, val = 30},
		{name = "willpower", max = 100, val = 50}},
	speed_walk = 3,
	speed_run = 6,
	tilt_bone = "back",
	tilt_limit = 1.25}

Species{
	name = "kraken-player",
	base = "kraken",
	ai_enabled = false,
	inventory_items = {"torch", "mattock", "chainmail tunic", "chainmail pants", "chainmail leggings"}}

Species{
	name = "wyrm",
	equipment_slots = {
		{name = "arms"},
		{name = "feet"},
		{name = "hand.L", node = "#hand.L"},
		{name = "hand.R", node = "#hand.R"},
		{name = "head"},
		{name = "lowerbody"},
		{name = "upperbody"}},
	factions = {"good"},
	feats = {"attack"},
	inventory_size = 20,
	model = "aer1",
	models = { -- TODO: Models needed
		skeleton = "aer1",
		arms = "aerarmnude1",
		head = "wyrmhead1",
		legs = "aerlegnude1",
		lower = "wyrmlowernude1",
		upper = "aeruppernude1"},
	skill_quota = 200,
	skills = {
		{name = "dexterity", max = 40, val = 20},
		{name = "health", max = 100, val = 50},
		{name = "intelligence", max = 80, val = 30},
		{name = "perception", max = 80, val = 30},
		{name = "strength", max = 100, val = 50},
		{name = "willpower", max = 60, val = 20}},
	speed_walk = 3,
	speed_run = 6,
	tilt_bone = "back",
	tilt_limit = 1.25}

Species{
	name = "wyrm-player",
	base = "wyrm",
	ai_enabled = false,
	inventory_items = {"torch", "mattock", "chainmail tunic", "chainmail pants", "chainmail leggings"}}

Species{
	name = "companion",
	base = "aer",
	dialog = "companion",
	hair_style = {"hair1",255,255,255},
	inventory_items = {"torch", "axe", "chainmail tunic", "chainmail pants", "chainmail leggings"}}

Species{
	name = "brigand",
	base = "aer",
	inventory_items = {"axe", "leather top", "leather pants", "leather leggings"},
	factions = {"evil"}}

Species{
	name = "lipscitizen",
	base = "aer",
	dialog = "lipscitizen",
	hair_style = {"hair1",255,192,192},
	inventory_items = {"leather top", "leather pants", "leather leggings"}}

Species{
	name = "bloodworm",
	ai_enable_strafe = false,
	ai_enable_defend = false,
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

Species{
	name = "seirei",
	model = "seirei1",
	ai_enable_combat = false,
	ai_enable_wander = false,
	factions = {"neutral", "seirei"}}
