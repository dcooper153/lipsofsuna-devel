Featanimspec{
	name = "build",
	animation = "build",
	categories = {"build"},
	cooldown = 0.5,
	effect = "swing1",
	required_weapon = "build",
	slot = "hand.R"}

Featanimspec{
	name = "right hand",
	animation = "attack punch",
	bonuses_barehanded = true,
	bonuses_weapon = true,
	categories = {"melee"},
	cooldown = 0.1,
	effect = "swing1",
	effect_impact = "impact1",
	required_weapon = "melee",
	slot = "hand.R"}

Featanimspec{
	name = "ranged",
	animation = "attack bow",
	bonuses_projectile = true,
	bonuses_weapon = true,
	categories = {"ranged"},
	cooldown = 0.5,
	effect_impact = "impact1",
	required_ammo = true,
	required_weapon = "ranged",
	slot = "hand.R"}

Featanimspec{
	name = "ranged spell",
	animation = "spell ranged",
	categories = {"ranged spell"},
	cooldown = 2,
	description = "Cast a magical projectile",
	effect = "spell1"}

Featanimspec{
	name = "spell on self",
	animation = "spell self",
	categories = {"spell on self"},
	cooldown = 2,
	description = "Cast a spell on yourself",
	effect = "spell1"}

Featanimspec{
	name = "spell on touch",
	animation = "spell touch",
	categories = {"spell on touch"},
	cooldown = 2,
	description = "Cast a short range touch spell",
	effect = "spell1"}

Featanimspec{
	name = "throw",
	animation = "throw",
	bonuses_weapon = true,
	categories = {"throw"},
	cooldown = 1,
	effect = "swing1",
	effect_impact = "impact1",
	required_weapon = "throwable",
	slot = "hand.R"}

---------------------------------------------------------------------
-- Effects usable to players.

Feateffectspec{
	name = "berserk",
	animations = {"spell on self", "spell on touch"},
	categories = {"beneficial", "berserk", "spell"},
	description = "Inflict additional damage when in low health",
	effect = "berserk1",
	icon = "modifier-berserk",
	influences = {{"berserk", 30, 2}},
	skill_base = {willpower = 5},
	skill_mult = {willpower = 0.1},
	reagent_base = {["milky powder"] = 0.5},
	reagent_mult = {["milky powder"] = 0.02}}

Feateffectspec{
	name = "black haze",
	animations = {"ranged spell"},
	categories = {"harmful", "plague", "spell"},
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
	name = "dig",
	animations = {"ranged spell"},
	categories = {"dig", "spell"},
	description = "Fire a digging ray",
	effect = "spell1",
	icon = "modifier-dig",
	influences = {{"dig", 1, 1}},
	projectile = "fireball1", -- FIXME
	skill_base = {willpower = 3},
	skill_mult = {willpower = 0.3},
	reagent_base = {["milky powder"] = 0.5},
	reagent_mult = {["milky powder"] = 0.02}}

Feateffectspec{
	name = "fire damage",
	animations = {"ranged spell", "spell on self", "spell on touch"},
	categories = {"fire", "harmful", "melee", "spell"},
	description = "Inflict fire damage",
	effect = "explosion1",
	icon = "modifier-fireball",
	influences = {{"fire", -5, -0.95}},
	projectile = "fireball1",
	skill_mult = {intelligence = 1, willpower = 1},
	reagent_base = {["milky powder"] = 1},
	reagent_mult = {["milky powder"] = 0.1}}

Feateffectspec{
	name = "firewall",
	animations = {"ranged spell"},
	categories = {"fire", "harmful", "spell"},
	description = "Conjure a wall of fire",
	effect = "firewall1",
	icon = "modifier-firewall",
	influences = {{"firewall", 5, 0.25}},
	projectile = "fireball1", -- FIXME
	skill_mult = {intelligence = 1, willpower = 1},
	reagent_base = {["milky powder"] = 1},
	reagent_mult = {["milky powder"] = 0.1}}

Feateffectspec{
	name = "light",
	animations = {"spell on self"},
	categories = {"beneficial", "light", "spell"},
	description = "Illuminate your surroundings",
	effect = "light1",
	icon = "modifier-light",
	influences = {{"light", 30, 2}},
	skill_base = {willpower = 5},
	skill_mult = {willpower = 0.1},
	reagent_base = {["milky powder"] = 0.5},
	reagent_mult = {["milky powder"] = 0.02}}

Feateffectspec{
	name = "physical damage",
	animations = {"ranged", "right hand"},
	categories = {"harmful", "melee", "physical"},
	effect = "impact1",
	icon = "modifier-physical",
	influences = {{"physical", -1, -0.2}},
	skill_mult = {strength = 1, dexterity = 0.5}}

Feateffectspec{
	name = "restore health",
	animations = {"spell on self", "spell on touch"},
	categories = {"beneficial", "heal", "spell"},
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
	animations = {"spell on self", "spell on touch"},
	categories = {"beneficial", "spell"},
	description = "Protect from death",
	effect = "sanctuary1",
	icon = "modifier-sanctuary",
	influences = {{"sanctuary", 30, 2}},
	reagent_base = {["milky powder"] = 3},
	skill_base = {intelligence = 20, willpower = 20}}

Feateffectspec{
	name = "travel",
	animations = {"spell on self"},
	categories = {"spell"},
	description = "Fast travel to Lips",
	effect = "sanctuary1", -- FIXME
	icon = "modifier-sanctuary", -- FIXME
	influences = {{"travel", 30, 2}},
	reagent_base = {["dewspring leaf"] = 1},
	skill_base = {intelligence = 10, willpower = 10}}

Feateffectspec{
	name = "wilderness",
	animations = {"spell on self"},
	categories = {"spell"},
	description = "Grow plants from soil",
	effect = "wilderness1",
	icon = "modifier-nature",
	influences = {{"wilderness", 5, 1.06}},
	reagent_base = {["milky powder"] = 1},
	reagent_mult = {["milky powder"] = 0.09},
	skill_base = {intelligence = 10, willpower = 5},
	skill_mult = {intelligence = 1, willpower = 0.1}}

---------------------------------------------------------------------
-- Animations and effects usable to monsters only.

Featanimspec{
	name = "bite",
	animation = "attack bite",
	categories = {"melee"},
	cooldown = 2,
	effect = "growl1",
	effect_impact = "impact1",
	influences = {{"physical", -1}},
	slot = "mouth"}

Featanimspec{
	name = "dragon bite",
	action_frames = {0, 30},
	animation = "attack bite",
	categories = {"melee"},
	cooldown = 4,
	effect_impact = "impact1",
	influences = {{"physical", -50}},
	slot = "mouth"}

Featanimspec{
	name = "explode",
	animation = "explode",
	categories = {"explode", "melee"},
	cooldown = 2,
	influences = {{"physical", -1}},
	effect = "selfdestruct1"}

Featanimspec{
	name = "left claw",
	action_frames = {0, 15},
	animation = "attack left claw",
	categories = {"melee"},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {{"physical", -30}},
	slot = "hand.L"}

Featanimspec{
	name = "right claw",
	action_frames = {0, 15},
	animation = "attack right claw",
	categories = {"melee"},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {{"physical", -30}},
	slot = "hand.R"}

Feateffectspec{
	name = "dexterity to physical damage",
	categories = {"harmful", "melee", "physical"},
	effect = "impact1",
	icon = "modifier-physical",
	influences = {{"physical", -1, -0.2}},
	skill_mult = {dexterity = 1}}

Feateffectspec{
	name = "strength to physical damage",
	animations = {"bite", "dragon bite", "explode", "left claw", "right claw"},
	categories = {"harmful", "melee", "physical"},
	effect = "impact1",
	icon = "modifier-physical",
	influences = {{"physical", -1, -0.2}},
	skill_mult = {strength = 1}}
