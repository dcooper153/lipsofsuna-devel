Feat{
	name = "attack",
	inflict_damage = 5}

Feat{
	name = "attack-melee",
	categories = {"melee"},
	cooldown = 1,
	effect_animation = "punch",
	effect_sound = "swing1",
	inflict_damage = 20,
	required_weapon = "melee",
	targeting_mode = "weapon"}

Feat{
	name = "attack-ranged",
	categories = {"ranged"},
	cooldown = 1,
	effect_animation = "fire-bow",
	inflict_damage = 30,
	required_ammo = {arrow = 1},
	required_weapon = "ranged",
	targeting_mode = "weapon"}

Feat{
	name = "attack-throw",
	categories = {"ranged"},
	cooldown = 1,
	effect_animation = "throw",
	inflict_damage = 30,
	required_weapon = "throwable",
	targeting_mode = "weapon"}

Feat{
	name = "bite",
	categories = {"melee"},
	cooldown = 2,
	effect_animation = "bite",
	effect_sound = "growl1",
	inflict_damage = 10}

Feat{
	name = "criticalhit",
	categories = {"melee"},
	cooldown = 3,
	effect_animation = "punch",
	effect_sound = "swing1",
	inflict_damage = 30,
	locked = true,
	required_skills = {strength = 20},
	required_weapon = "melee",
	targeting_mode = "weapon"}

Feat{
	name = "heal",
	cooldown = 2,
	effect_animation = "cast-spell",
	effect_sound = "heal1",
	inflict_damage = -10,
	required_reagents = {mushroom = 1},
	required_skills = {willpower = 10},
	targeting_mode = "self"}

Feat{
	name = "fireball",
	categories = {"ranged"},
	cooldown = 2,
	effect_animation = "cast-spell",
	inflict_damage = 20,
	required_reagents = {mushroom = 1},
	required_skills = {willpower = 10},
	targeting_mode = "bullet"}

Feat{
	name = "spinattack",
	categories = {"melee"},
	cooldown = 3,
	effect_animation = "spin-attack",
	effect_sound = "swing1",
	inflict_damage = 40,
	locked = true,
	required_skills = {strength = 30},
	required_weapon = "melee",
	targeting_mode = "weapon"}

Feat{
	name = "vectorchange",
	cooldown = 3,
	effect_animation = "cast-spell",
	effect_sound = "swing1",
	locked = true,
	radius = 50,
	required_reagents = {mushroom = 1},
	required_skills = {willpower = 30},
	targeting_mode = "self"}
