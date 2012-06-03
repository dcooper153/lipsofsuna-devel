---------------------------------------------------------------------
-- Types usable to players.

Feattypespec{
	name = "area spell",
	action = "area spell",
	animation = "spell ranged",
	categories = {["area spell"] = true},
	cooldown = 1,
	icon = "modifier-fireball", -- FIXME
	description = "Cast a spell that affects nearby targets.",
	effect = "spell1",
	range = 5}

Feattypespec{
	name = "build",
	action = "build",
	animation = "build",
	categories = {["build"] = true},
	cooldown = 0.5,
	effect = "swing1",
	required_weapon = "build"}

Feattypespec{
	name = "missile spell",
	action = "missile spell",
	animation = "spell ranged",
	categories = {["ranged spell"] = true},
	cooldown = 1,
	icon = "modifier-fireball", -- FIXME
	description = "Cast a magical missile that you can control.",
	effect = "spell1",
	range = 10}

Feattypespec{
	name = "right hand",
	action = "melee",
	animation = "attack punch",
	bonuses_barehanded = true,
	bonuses_weapon = true,
	categories = {["melee"] = true},
	cooldown = 0.8,
	directional = true,
	effect = "swing1",
	effect_impact = "impact1",
	required_weapon = "melee",
	slot = "hand.R"}

Feattypespec{
	name = "ranged",
	action = "ranged",
	animation = "attack bow",
	bonuses_projectile = true,
	bonuses_weapon = true,
	categories = {["ranged"] = true},
	cooldown = 1.2,
	effect_impact = "impact1",
	range = 10,
	required_ammo = true,
	required_weapon = "ranged"}

Feattypespec{
	name = "ranged spell",
	action = "ranged spell",
	animation = "spell ranged",
	categories = {["ranged spell"] = true},
	cooldown = 1,
	description = "Cast a magical projectile",
	effect = "spell1",
	range = 10}

Feattypespec{
	name = "spell on self",
	action = "spell on self",
	animation = "spell self",
	categories = {["spell on self"] = true},
	cooldown = 1,
	description = "Cast a spell on yourself",
	effect = "spell1"}

Feattypespec{
	name = "spell on touch",
	action = "spell on touch",
	animation = "spell touch",
	categories = {["spell on touch"] = true},
	cooldown = 1,
	description = "Cast a short range touch spell",
	effect = "spell1"}

Feattypespec{
	name = "throw",
	action = "throw",
	animation = "throw",
	bonuses_weapon = true,
	categories = {["throw"] = true},
	cooldown = 1,
	effect = "swing1",
	effect_impact = "impact1",
	range = 10,
	required_weapon = "throwable"}

---------------------------------------------------------------------
-- Types usable to monsters only.

Feattypespec{
	name = "bite",
	action = "melee",
	animation = "attack bite",
	categories = {["melee"] = true},
	cooldown = 2,
	effect = "growl1",
	effect_impact = "impact1",
	influences = {["physical damage"] = 1},
	slot = "mouth"}

Feattypespec{
	name = "dragon bite",
	action = "melee",
	animation = "attack bite",
	categories = {["melee"] = true},
	cooldown = 4,
	effect_impact = "impact1",
	influences = {["physical damage"] = 50},
	slot = "mouth"}

Feattypespec{
	name = "explode",
	action = "explode",
	animation = "explode",
	categories = {["explode"] = true, ["melee"] = true},
	cooldown = 2,
	influences = {["physical damage"] = 1},
	effect = "selfdestruct1"}

Feattypespec{
	name = "left claw",
	action = "melee",
	animation = "attack left claw",
	categories = {["melee"] = true},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {["physical damage"] = 5},
	slot = "hand.L"}

Feattypespec{
	name = "right claw",
	action = "melee",
	animation = "attack right claw",
	categories = {["melee"] = true},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {["physical damage"] = 5},
	slot = "hand.R"}

Feattypespec{
	name = "dragon left claw",
	action = "melee",
	animation = "attack left claw",
	categories = {["melee"] = true},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {["physical damage"] = 30},
	slot = "hand.L"}

Feattypespec{
	name = "dragon right claw",
	action = "melee",
	animation = "attack right claw",
	categories = {["melee"] = true},
	cooldown = 2,
	effect = "swing1",
	effect_impact = "impact1",
	influences = {["physical damage"] = 30},
	slot = "hand.R"}

Feattypespec{
	name = "tackle",
	action = "tackle",
	animation = "tackle",
	categories = {["melee"] = true},
	cooldown = 5,
	effect = "swing1",
	influences = {["physical damage"] = 5},
	range = 5}

---------------------------------------------------------------------
-- Effects usable to players.

Feateffectspec{
	name = "berserk",
	categories = {["beneficial"] = true, ["berserk"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on self"] = true, ["spell on touch"] = true},
	description = "Inflict additional damage when in low health",
	effect = "berserk1",
	icon = "modifier-berserk",
	influences = {["berserk"] = 60},
	required_stats = {["willpower"] = 5},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "black haze",
	categories = {["harmful"] = true, ["plague"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true},
	description = "Conjure infectious plagued monsters",
	effect = "spell1",
	icon = "modifier-black haze",
	influences = {["black haze"] = 1},
	projectile = "fireball1",
	required_stats = {["willpower"] = 20},
	required_reagents = {["mushroom"] = 1}}

Feateffectspec{
	name = "bless",
	categories = {["beneficial"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on self"] = true, ["spell on touch"] = true},
	description = "Boost the maximum health and willpower of the target",
	effect = "spell1",
	icon = "modifier-heal", --FIXME
	influences = {["bless"] = 60},
	projectile = "fireball1",
	required_stats = {["willpower"] = 10},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "curse",
	categories = {["harmful"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on self"] = true, ["spell on touch"] = true},
	description = "Damages the maximum health and willpower of the target",
	effect = "spell1",
	icon = "firewall", --FIXME
	influences = {["curse"] = 60},
	projectile = "fireball1",
	required_stats = {["willpower"] = 10},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "burning",
	categories = {["harmful"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on self"] = true, ["spell on touch"] = true},
	description = "Inflict damage over time by putting the target on fire",
	effect = "berserk1",
	icon = "firewall", -- FIXME
	influences = {["burning"] = 10},
	required_stats = {["willpower"] = 5},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "cold damage",
	categories = {["cold"] = true, ["harmful"] = true, ["melee"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on self"] = true, ["spell on touch"] = true},
	description = "Inflict cold damage",
	effect = "explosion1", -- FIXME
	icon = "modifier-ice",
	influences = {["cold damage"] = 5},
	projectile = "fireball1", -- FIXME
	required_stats = {["willpower"] = 1},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "cure disease",
	categories = {["beneficial"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on self"] = true, ["spell on touch"] = true},
	description = "Cure the target of poison and diseases",
	effect = "spell1",
	icon = "modifier-heal", --FIXME
	influences = {["cure disease"] = 1},
	projectile = "fireball1",
	required_stats = {["willpower"] = 10},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "dig",
	categories = {["dig"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true},
	description = "Fire a digging ray",
	effect = "spell1",
	icon = "modifier-earthmove",
	influences = {["dig"] = 1},
	projectile = "fireball1", -- FIXME
	required_stats = {["willpower"] = 3},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "fire damage",
	categories = {["fire"] = true, ["harmful"] = true, ["melee"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on self"] = true, ["spell on touch"] = true},
	description = "Inflict fire damage",
	effect = "explosion1",
	icon = "modifier-fireball",
	influences = {["fire damage"] = 5},
	projectile = "fireball1",
	required_stats = {["willpower"] = 1},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "fire elemental",
	categories = {["spell"] = true, ["summon"] = true},
	animations = {["spell on self"] = true},
	description = "Conjure a fire elemental",
	effect = "spell1", --FIXME
	icon = "firewall", --FIXME
	influences = {["fire elemental"] = 60},
	required_stats = {["willpower"] = 10},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "firewall",
	categories = {["fire"] = true, ["harmful"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true},
	description = "Conjure a wall of fire",
	effect = "firewall1",
	icon = "firewall",
	influences = {["firewall"] = 5},
	projectile = "fireball1", -- FIXME
	required_stats = {["willpower"] = 1},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "frost vortex",
	categories = {["harmful"] = true, ["spell"] = true},
	animations = {["area spell"] = true},
	description = "Inflict cold damage to nearby targets",
	effect = "spell1",
	icon = "modifier-ice",
	radius = 5,
	duration = 10,
	influences = {["cold damage"] = 5},
	projectile = "fireball1",
	required_stats = {["willpower"] = 10},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "light",
	categories = {["beneficial"] = true, ["light"] = true, ["spell"] = true},
	animations = {["spell on self"] = true},
	description = "Illuminate your surroundings",
	effect = "light1",
	icon = "modifier-light",
	influences = {["light"] = 60},
	required_stats = {["willpower"] = 5},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "mindless march",
	categories = {["harmful"] = true, ["push"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on touch"] = true},
	description = "Forces the target to march forward",
	effect = "light1",
	icon = "modifier-light",
	influences = {["mindless march"] = 30},
	required_stats = {["willpower"] = 5},
	projectile = "magicmissile1",
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "follow",
	categories = {["follow"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on touch"] = true},
	description = "Forces the target to follow you",
	effect = "light1",
	icon = "modifier-light",
	influences = {["follow"] = 30},
	required_stats = {["willpower"] = 5},
	projectile = "magicmissile1",
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "home",
	categories = {["follow"] = true, ["spell"] = true},
	animations = {["spell on self"] = true},
	description = "Creates a home location where you can respawn from",
	effect = "light1",
	icon = "sanctuary1",
	influences = {["home"] = 1},
	required_stats = {["willpower"] = 1},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "physical damage",
	categories = {["harmful"] = true, ["melee"] = true, ["physical"] = true},
	animations = {["bite"] = true, ["dragon bite"] = true, ["explode"] = true, ["left claw"] = true, ["ranged"] = true, ["right hand"] = true, ["right claw"] = true, ["tackle"] = true},
	effect = "impact1",
	icon = "missing1", --FIXME
	influences = {["physical damage"] = 1}}

Feateffectspec{
	name = "poison",
	categories = {["harmful"] = true, ["plague"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true},
	description = "The target takes poison damage over time",
	effect = "spell1",
	icon = "modifier-black haze", --FIXME
	influences = {["poison"] = 10},
	projectile = "fireball1",
	required_stats = {["willpower"] = 10},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "regeneration",
	categories = {["beneficial"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on self"] = true, ["spell on touch"] = true},
	description = "Regenerate health every second",
	effect = "berserk1",
	icon = "modifier-heal",
	influences = {["regeneration"] = 60},
	required_stats = {["willpower"] = 15},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "restore health",
	categories = {["beneficial"] = true, ["heal"] = true, ["spell"] = true},
	animations = {["spell on self"] = true, ["spell on touch"] = true},
	description = "Restore health",
	effect = "heal1",
	icon = "modifier-heal",
	influences = {["restore health"] = 15},
	required_stats = {["willpower"] = 5},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "resurrect",
	categories = {["beneficial"] = true, ["spell"] = true},
	animations = {["missile spell"] = true, ["ranged spell"] = true, ["spell on touch"] = true},
	description = "Resurrect the targeted creature",
	effect = "heal1",
	icon = "modifier-heal",
	influences = {["resurrect"] = 1},
	required_stats = {["willpower"] = 20},
	required_reagents = {["dewspring leaf"] = 1}}

Feateffectspec{
	name = "restore willpower",
	categories = {["beneficial"] = true, ["spell"] = true},
	animations = {["spell on self"] = true, ["spell on touch"] = true},
	description = "Restore health",
	effect = "heal1",
	icon = "modifier-heal", --FIXME
	influences = {["restore willpower"] = 15},
	required_stats = {["willpower"] = 5},
	required_reagents = {["milky powder"] = 1}}

Feateffectspec{
	name = "sanctuary",
	animations = {["spell on self"] = true, ["spell on touch"] = true},
	categories = {["beneficial"] = true, ["spell"] = true},
	description = "Protect from death",
	effect = "sanctuary1",
	icon = "sanctuary1",
	influences = {["sanctuary"] = 30},
	required_reagents = {["milky powder"] = 3},
	required_stats = {["willpower"] = 20}}

Feateffectspec{
	name = "travel",
	animations = {["spell on self"] = true},
	categories = {["spell"] = true},
	description = "Fast travel to Lips",
	effect = "sanctuary1", -- FIXME
	icon = "sanctuary1", -- FIXME
	influences = {["travel"] = 30},
	required_reagents = {["dewspring leaf"] = 1},
	required_stats = {["willpower"] = 10}}

Feateffectspec{
	name = "wilderness",
	animations = {["spell on self"] = true},
	categories = {["spell"] = true},
	description = "Grow plants from soil",
	effect = "wilderness1",
	icon = "modifier-nature",
	influences = {["wilderness"] = 5},
	required_reagents = {["milky powder"] = 1},
	required_stats = {["willpower"] = 5}}

---------------------------------------------------------------------
-- Effects usable to monsters only.

Feateffectspec{
	name = "dragon breath",
	animations = {["ranged spell"] = true},
	categories = {["harmful"] = true, ["spell"] = true},
	effect = "dragonbreath1",
	icon = "modifier-fireball",
	influences = {["fire damage"] = 20},
	projectile = "firebreath1",
	required_stats = {["willpower"] = 5}}
