Featanimspec{
	name = "bite",
	animation = "bite",
	categories = {"melee"},
	cooldown = 2,
	effect = "growl1",
	effect_impact = "impact1",
	inflict_damage = 10,
	slot = "mouth"}

Featanimspec{
	name = "build",
	animation = "punch",
	categories = {"build"},
	cooldown = 0.5,
	effect = "swing1",
	required_weapon = "build",
	slot = "hand.R"}

Featanimspec{
	name = "dragon bite",
	action_frames = {0, 30},
	animation = "bite",
	categories = {"melee"},
	cooldown = 4,
	effect_impact = "impact1",
	inflict_damage = 50,
	slot = "mouth"}

Featanimspec{
	name = "left claw",
	action_frames = {0, 15},
	animation = "claw-left",
	categories = {"melee"},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	inflict_damage = 30,
	slot = "hand.L"}

Featanimspec{
	name = "right claw",
	action_frames = {0, 15},
	animation = "claw-right",
	categories = {"melee"},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	inflict_damage = 30,
	slot = "hand.R"}

Featanimspec{
	name = "right hand",
	animation = "punch",
	categories = {"melee"},
	cooldown = 0.5,
	effect = "swing1",
	effect_impact = "impact1",
	required_weapon = "melee",
	slot = "hand.R"}

Featanimspec{
	name = "ranged",
	action_frames = {10, 10},
	animation = "fire-bow",
	categories = {"ranged"},
	cooldown = 1,
	effect_impact = "impact1",
	required_ammo = {arrow = 1},
	required_weapon = "ranged",
	slot = "hand.R"}

Featanimspec{
	name = "ranged spell",
	animation = "cast-spell",
	categories = {"ranged", "spell"},
	cooldown = 2,
	effect = "spell1"}

Featanimspec{
	name = "spell on self",
	animation = "cast-spell",
	categories = {"self", "spell"},
	cooldown = 2,
	effect = "spell1"}

Featanimspec{
	name = "spell on touch",
	animation = "cast-spell",
	categories = {"spell", "touch"},
	cooldown = 2,
	effect = "spell1"}

Featanimspec{
	name = "throw",
	action_frames = {10, 10},
	animation = "throw",
	categories = {"throw"},
	cooldown = 1,
	effect = "swing1",
	effect_impact = "impact1",
	required_weapon = "throwable",
	slot = "hand.R"}

---------------------------------------------------------------------

Feateffectspec{
	name = "fire damage",
	categories = {"fire", "melee", "spell"},
	damage_base = 5,
	damage_mult = 0.95,
	effect = "explosion1",
	projectile = "fireball1",
	skill_mult = {intelligence = 1, willpower = 1},
	reagent_base = {mushroom = 1},
	reagent_mult = {mushroom = 0.1}}

Feateffectspec{
	name = "physical damage",
	categories = {"melee", "physical"},
	damage_base = 5,
	damage_mult = 0.95,
	effect = "impact1",
	skill_mult = {strength = 1, dexterity = 0.5}}

Feateffectspec{
	name = "restore health",
	categories = {"heal", "spell"},
	damage_base = -5,
	damage_mult = -1.06,
	effect = "heal1",
	skill_base = {intelligence = 10, willpower = 5},
	skill_mult = {intelligence = 1, willpower = 0.1},
	reagent_base = {mushroom = 1},
	reagent_mult = {mushroom = 0.09}}
