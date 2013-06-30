local Actor = require("core/objects/actor")

-- Summon plagued beasts.
Feateffectspec{
	name = "black haze",
	categories =
	{
		["harmful"] = true,
		["plague"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true
	},
	description = "Conjure infectious plagued monsters",
	effect = "spell1",
	icon = "modifier-black haze",
	influences = {["black haze"] = 1},
	projectile = "fireball1",
	required_stats = {["willpower"] = 20},
	modifier = function(self, mod, secs)
		-- Wait five seconds.
		mod.strength = 10000
		mod.timer = mod.timer + secs
		if mod.timer < 5 then return true end
		mod.timer = mod.timer - 5
		-- Damage the actor.
		mod.object:damaged{amount = 5, type = "disease"}
		-- Infect nearby actors.
		local near = Main.objects:find_by_point(mod.object:get_position(), 5)
		for k,v in pairs(near) do
			if math.random() > 0.1 then
				v:inflict_modifier("black haze", 10000)
			end
		end
		return true
	end,
	touch = function(self, args)
		local s = Actorspec:random{category = "plague"}
		if not s then return end
		local p = args.point + Vector(
			-1 + 2 * math.random(),
			-1 + 2 * math.random(),
			-1 + 2 * math.random())
		local o = Actor(Main.objects)
		o:set_spec(s)
		o:set_position(p)
		o:randomize()
		o:set_visible(true)
		o:inflict_modifier("black haze", 10000)
	end}
