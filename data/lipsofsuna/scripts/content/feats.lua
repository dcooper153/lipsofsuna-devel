Featanimspec{
	name = "bite",
	animation = "attack bite",
	categories = {"melee"},
	cooldown = 2,
	effect = "growl1",
	effect_impact = "impact1",
	influences = {{"health", -10}},
	slot = "mouth"}

Featanimspec{
	name = "build",
	animation = "build",
	categories = {"build"},
	cooldown = 0.5,
	effect = "swing1",
	required_weapon = "build",
	slot = "hand.R"}

Featanimspec{
	name = "dragon bite",
	action_frames = {0, 30},
	animation = "attack bite",
	categories = {"melee"},
	cooldown = 4,
	effect_impact = "impact1",
	influences = {{"health", -50}},
	slot = "mouth"}

Featanimspec{
	name = "left claw",
	action_frames = {0, 15},
	animation = "attack left claw",
	categories = {"melee"},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {{"health", -30}},
	slot = "hand.L"}

Featanimspec{
	name = "right claw",
	action_frames = {0, 15},
	animation = "attack right claw",
	categories = {"melee"},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {{"health", -30}},
	slot = "hand.R"}

Featanimspec{
	name = "right hand",
	animation = "attack fist",
	categories = {"melee"},
	description = "Hit with your right hand",
	cooldown = 1,
	effect = "swing1",
	effect_impact = "impact1",
	required_weapon = "melee",
	slot = "hand.R"}

Featanimspec{
	name = "ranged",
	animation = "attack bow",
	categories = {"ranged"},
	cooldown = 1,
	description = "Fire a ranged weapon",
	effect_impact = "impact1",
	required_ammo = {arrow = 1},
	required_weapon = "ranged",
	slot = "hand.R"}

Featanimspec{
	name = "ranged spell",
	animation = "spell ranged",
	categories = {"ranged", "spell"},
	cooldown = 2,
	description = "Cast a magical projectile",
	effect = "spell1"}

Featanimspec{
	name = "spell on self",
	animation = "spell self",
	categories = {"self", "spell"},
	cooldown = 2,
	description = "Cast a spell on yourself",
	effect = "spell1"}

Featanimspec{
	name = "spell on touch",
	animation = "spell touch",
	categories = {"spell", "touch"},
	cooldown = 2,
	description = "Cast a short range touch spell",
	effect = "spell1"}

Featanimspec{
	name = "throw",
	animation = "throw",
	categories = {"throw"},
	cooldown = 1,
	effect = "swing1",
	effect_impact = "impact1",
	required_weapon = "throwable",
	slot = "hand.R"}

---------------------------------------------------------------------

Feateffectspec{
	name = "black haze",
	categories = {"plague", "spell"},
	description = "Conjure infectious plagued monsters",
	effect = "spell1",
	icon = "modifier-plague",
	influences = {{"plague", 1, 0.04}},
	projectile = "fireball1",
	skill_base = {willpower = 25},
	skill_mult = {willpower = 0.2},
	reagent_base = {mushroom = 0.5},
	reagent_mult = {mushroom = 0.02}}

Feateffectspec{
	name = "fire damage",
	categories = {"fire", "melee", "spell"},
	description = "Inflict fire damage",
	effect = "explosion1",
	influences = {{"fire", -5, -0.95}},
	projectile = "fireball1",
	skill_mult = {intelligence = 1, willpower = 1},
	reagent_base = {["milky powder"] = 1},
	reagent_mult = {["milky powder"] = 0.1}}

Feateffectspec{
	name = "physical damage",
	categories = {"melee", "physical"},
	description = "Inflict physical or telekinetic damage",
	effect = "impact1",
	influences = {{"physical", -5, -0.95}},
	skill_mult = {strength = 1, dexterity = 0.5}}

Feateffectspec{
	name = "restore health",
	categories = {"heal", "spell"},
	description = "Restore health",
	effect = "heal1",
	icon = "modifier-heal",
	influences = {{"physical", 5, 1.06}},
	skill_base = {intelligence = 10, willpower = 5},
	skill_mult = {intelligence = 1, willpower = 0.1},
	reagent_base = {["milky powder"] = 1},
	reagent_mult = {["milky powder"] = 0.09}}

Feateffectspec{
	name = "sanctuary",
	categories = {"spell"},
	description = "Protect from death",
	effect = "sanctuary1",
	icon = "modifier-sanctuary",
	influences = {{"sanctuary", 30, 2}},
	reagent_base = {["milky powder"] = 3},
	skill_base = {intelligence = 20, willpower = 20}}

Feateffectspec{
	name = "wilderness",
	categories = {"spell"},
	description = "Grow plants from soil",
	effect = "wilderness1",
	icon = "wilderness1",
	influences = {{"wilderness", 5, 1.06}},
	reagent_base = {["milky powder"] = 1},
	reagent_mult = {["milky powder"] = 0.09},
	skill_base = {intelligence = 10, willpower = 5},
	skill_mult = {intelligence = 1, willpower = 0.1}}
