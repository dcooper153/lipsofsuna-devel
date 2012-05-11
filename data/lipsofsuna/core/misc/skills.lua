Skillspec{
	name = "Health lv1",
	description = "Increase your base health to 40 points.",
	icon = "modifier-heal",
	assign = function(skills) skills.max_health = skills.max_health + 20 end}

Skillspec{
	name = "Health lv2",
	description = "Increase your base health to 60 points.",
	icon = "modifier-heal",
	requires = {"Health lv1"},
	assign = function(skills) skills.max_health = skills.max_health + 20 end}

Skillspec{
	name = "Health lv3",
	description = "Increase your base health to 80 points.",
	icon = "modifier-heal",
	requires = {"Health lv2"},
	assign = function(skills) skills.max_health = skills.max_health + 20 end}

Skillspec{
	name = "Health lv4",
	description = "Increase your base health to 100 points.",
	icon = "modifier-heal",
	requires = {"Health lv3"},
	assign = function(skills) skills.max_health = skills.max_health + 20 end}

Skillspec{
	name = "Willpower lv1",
	description = "Increase your base willpower to 40 points.",
	icon = "modifier-mana",
	assign = function(skills) skills.max_willpower = skills.max_willpower + 20 end}

Skillspec{
	name = "Willpower lv2",
	description = "Increase your base willpower to 60 points.",
	icon = "modifier-mana",
	requires = {"Willpower lv1"},
	assign = function(skills) skills.max_willpower = skills.max_willpower + 20 end}

Skillspec{
	name = "Willpower lv3",
	description = "Increase your base willpower to 80 points.",
	icon = "modifier-mana",
	requires = {"Willpower lv2"},
	assign = function(skills) skills.max_willpower = skills.max_willpower + 20 end}

Skillspec{
	name = "Willpower lv4",
	description = "Increase your base willpower to 100 points.",
	icon = "modifier-mana",
	requires = {"Willpower lv3"},
	assign = function(skills) skills.max_willpower = skills.max_willpower + 20 end}

Skillspec{
	name = "Sprinter",
	description = "Run and walk twice as fast.",
	assign = function(skills) skills.speed = skills.speed * 2 end}

Skillspec{
	name = "Far sight",
	description = "Spot items and actors twice as far.",
	assign = function(skills) skills.view_distance = skills.view_distance * 2 end}

Skillspec{
	name = "Fighter",
	description = "Melee attacks do double damage.",
	assign = function(skills) skills.melee = skills.melee * 2 end}

Skillspec{
	name = "Marksman",
	description = "Ranged weapons do double damage.",
	assign = function(skills) skills.ranged = skills.ranged * 2 end}

Skillspec{
	name = "Giant leap",
	description = "Jump higher.",
	assign = function(skills) skills.jump = skills.jump * 1.5 end}

Skillspec{
	name = "Soft landing",
	description = "Falling damage is halved.",
	assign = function(skills) skills.falling_damage = skills.falling_damage / 2 end}

------------------------------------------------------------------------------
-- Skills for monsters.

Skillspec{
	name = "Nerf combat lv1",
	assign = function(skills)
		skills.ranged = skills.ranged / 2
		skills.melee = skills.melee / 2
	end}

Skillspec{
	name = "Nerf combat lv2",
	requires = {"Nerf combat lv1"},
	assign = function(skills)
		skills.ranged = skills.ranged / 2
		skills.melee = skills.melee / 2
	end}

Skillspec{
	name = "Nerf health lv1",
	assign = function(skills) skills.max_health = skills.max_health / 2 end}

Skillspec{
	name = "Nerf health lv2",
	requires = {"Nerf health lv1"},
	assign = function(skills) skills.max_health = skills.max_health / 2 end}
