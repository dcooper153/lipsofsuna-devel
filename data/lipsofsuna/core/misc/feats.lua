Featanimspec{
	name = "area spell",
	action = "area spell",
	animation = "spell ranged",
	categories = {"area spell"},
	cooldown = 1,
	icon = "modifier-fireball", -- FIXME
	description = "Cast a spell that affects nearby targets.",
	effect = "spell1"}

Featanimspec{
	name = "build",
	action = "build",
	animation = "build",
	categories = {"build"},
	cooldown = 0.5,
	effect = "swing1",
	required_weapon = "build"}

Featanimspec{
	name = "missile spell",
	action = "missile spell",
	animation = "spell ranged",
	categories = {"ranged spell"},
	cooldown = 1,
	icon = "modifier-fireball", -- FIXME
	description = "Cast a magical missile that you can control.",
	effect = "spell1"}

Featanimspec{
	name = "right hand",
	action = "melee",
	animation = "attack punch",
	bonuses_barehanded = true,
	bonuses_weapon = true,
	categories = {"melee"},
	cooldown = 0.8,
	effect = "swing1",
	effect_impact = "impact1",
	required_weapon = "melee",
	slot = "hand.R"}

Featanimspec{
	name = "ranged",
	action = "ranged",
	animation = "attack bow",
	bonuses_projectile = true,
	bonuses_weapon = true,
	categories = {"ranged"},
	cooldown = 1.2,
	effect_impact = "impact1",
	required_ammo = true,
	required_weapon = "ranged"}

Featanimspec{
	name = "ranged spell",
	action = "ranged spell",
	animation = "spell ranged",
	categories = {"ranged spell"},
	cooldown = 1,
	description = "Cast a magical projectile",
	effect = "spell1"}

Featanimspec{
	name = "spell on self",
	action = "spell on self",
	animation = "spell self",
	categories = {"spell on self"},
	cooldown = 1,
	description = "Cast a spell on yourself",
	effect = "spell1"}

Featanimspec{
	name = "spell on touch",
	action = "spell on touch",
	animation = "spell touch",
	categories = {"spell on touch"},
	cooldown = 1,
	description = "Cast a short range touch spell",
	effect = "spell1"}

Featanimspec{
	name = "throw",
	action = "throw",
	animation = "throw",
	bonuses_weapon = true,
	categories = {"throw"},
	cooldown = 1,
	effect = "swing1",
	effect_impact = "impact1",
	required_weapon = "throwable"}

---------------------------------------------------------------------
-- Effects usable to players.

Feateffectspec{
	name = "berserk",
	animations = {"missile spell", "ranged spell", "spell on self", "spell on touch"},
	categories = {"beneficial", "berserk", "spell"},
	description = "Inflict additional damage when in low health",
	effect = "berserk1",
	icon = "modifier-berserk",
	influences = {{"berserk", 60}},
	skill_base = {willpower = 5},
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "black haze",
	animations = {"missile spell", "ranged spell"},
	categories = {"harmful", "plague", "spell"},
	description = "Conjure infectious plagued monsters",
	effect = "spell1",
	icon = "modifier-black haze",
	influences = {{"black haze", 1}},
	projectile = "fireball1",
	skill_base = {willpower = 20},
	reagent_base = {mushroom = 1}}

Feateffectspec{
	name = "burning",
	animations = {"missile spell", "ranged spell", "spell on self", "spell on touch"},
	categories = {"harmful", "spell"},
	description = "Inflict damage over time by putting the target on fire",
	effect = "berserk1",
	icon = "modifier-firewall", -- FIXME
	influences = {{"burning", 10}},
	skill_base = {willpower = 5},
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "dig",
	animations = {"missile spell", "ranged spell"},
	categories = {"dig", "spell"},
	description = "Fire a digging ray",
	effect = "spell1",
	icon = "modifier-earthmove",
	influences = {{"dig", 1}},
	projectile = "fireball1", -- FIXME
	skill_base = {willpower = 3},
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "fire damage",
	animations = {"missile spell", "ranged spell", "spell on self", "spell on touch"},
	categories = {"fire", "harmful", "melee", "spell"},
	description = "Inflict fire damage",
	effect = "explosion1",
	icon = "modifier-fireball",
	influences = {{"fire damage", -5}},
	projectile = "fireball1",
	skill_base = {willpower = 1},
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "firewall",
	animations = {"missile spell", "ranged spell"},
	categories = {"fire", "harmful", "spell"},
	description = "Conjure a wall of fire",
	effect = "firewall1",
	icon = "modifier-firewall",
	influences = {{"firewall", 5}},
	projectile = "fireball1", -- FIXME
	skill_mult = {willpower = 1},
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "light",
	animations = {"spell on self"},
	categories = {"beneficial", "light", "spell"},
	description = "Illuminate your surroundings",
	effect = "light1",
	icon = "modifier-light",
	influences = {{"light", 60}},
	skill_base = {willpower = 5},
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "mindless march",
	animations = {"missile spell", "ranged spell", "spell on touch"},
	categories = {"harmful", "push", "spell"},
	description = "Forces the target to march forward",
	effect = "light1",
	icon = "modifier-light",
	influences = {{"mindless march", 30}},
	skill_base = {willpower = 5},
	projectile = "magicmissile1",
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "follow",
	animations = {"missile spell", "ranged spell", "spell on touch"},
	categories = {"follow", "spell"},
	description = "Forces the target to follow you",
	effect = "light1",
	icon = "modifier-light",
	influences = {{"follow", 30}},
	skill_base = {willpower = 5},
	projectile = "magicmissile1",
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "home",
	animations = {"spell on self"},
	categories = {"follow", "spell"},
	description = "Creates a home location where you can respawn from",
	effect = "light1",
	icon = "sanctuary1",
	influences = {{"home", 1}},
	skill_base = {willpower = 1},
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "physical damage",
	animations = {"ranged", "right hand"},
	categories = {"harmful", "melee", "physical"},
	effect = "impact1",
	icon = "modifier-physical",
	influences = {{"physical damage", -1}}}

Feateffectspec{
	name = "restore health",
	animations = {"spell on self", "spell on touch"},
	categories = {"beneficial", "heal", "spell"},
	description = "Restore health",
	effect = "heal1",
	icon = "modifier-heal",
	influences = {{"physical damage", 15}},
	skill_base = {willpower = 5},
	reagent_base = {["milky powder"] = 1}}

Feateffectspec{
	name = "sanctuary",
	animations = {"spell on self", "spell on touch"},
	categories = {"beneficial", "spell"},
	description = "Protect from death",
	effect = "sanctuary1",
	icon = "modifier-sanctuary",
	influences = {{"sanctuary", 30}},
	reagent_base = {["milky powder"] = 3},
	skill_base = {willpower = 20}}

Feateffectspec{
	name = "travel",
	animations = {"spell on self"},
	categories = {"spell"},
	description = "Fast travel to Lips",
	effect = "sanctuary1", -- FIXME
	icon = "modifier-sanctuary", -- FIXME
	influences = {{"travel", 30}},
	reagent_base = {["dewspring leaf"] = 1},
	skill_base = {willpower = 10}}

Feateffectspec{
	name = "wilderness",
	animations = {"spell on self"},
	categories = {"spell"},
	description = "Grow plants from soil",
	effect = "wilderness1",
	icon = "modifier-nature",
	influences = {{"wilderness", 5}},
	reagent_base = {["milky powder"] = 1},
	skill_base = {willpower = 5}}

---------------------------------------------------------------------
-- Animations and effects usable to monsters only.

Featanimspec{
	name = "bite",
	action = "melee custom",
	animation = "attack bite",
	categories = {"melee"},
	cooldown = 2,
	effect = "growl1",
	effect_impact = "impact1",
	influences = {{"physical damage", -1}},
	slot = "mouth"}

Featanimspec{
	name = "dragon bite",
	action = "melee custom",
	action_frames = {0, 30},
	animation = "attack bite",
	categories = {"melee"},
	cooldown = 4,
	effect_impact = "impact1",
	influences = {{"physical damage", -50}},
	slot = "mouth"}

Featanimspec{
	name = "explode",
	action = "explode",
	animation = "explode",
	categories = {"explode", "melee"},
	cooldown = 2,
	influences = {{"physical damage", -1}},
	effect = "selfdestruct1"}

Featanimspec{
	name = "left claw",
	action = "melee custom",
	action_frames = {0, 15},
	animation = "attack left claw",
	categories = {"melee"},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {{"physical damage", -30}},
	slot = "hand.L"}

Featanimspec{
	name = "right claw",
	action = "melee custom",
	action_frames = {0, 15},
	animation = "attack right claw",
	categories = {"melee"},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {{"physical damage", -30}},
	slot = "hand.R"}

Feateffectspec{
	name = "dexterity to physical damage",
	categories = {"harmful", "melee custom", "physical"},
	effect = "impact1",
	icon = "modifier-physical",
	influences = {{"physical damage", -1}},
	skill_mult = {dexterity = 1}}

Feateffectspec{
	name = "dragon breath",
	animations = {"ranged spell"},
	categories = {"harmful", "spell"},
	effect = "dragonbreath1",
	icon = "modifier-fireball",
	influences = {{"fire damage", -20}},
	projectile = "firebreath1",
	skill_base = {willpower = 5}}

Feateffectspec{
	name = "strength to physical damage",
	animations = {"bite", "dragon bite", "explode", "left claw", "right claw"},
	categories = {"harmful", "melee custom", "physical"},
	effect = "impact1",
	icon = "modifier-physical",
	influences = {{"physical damage", -1}}}
