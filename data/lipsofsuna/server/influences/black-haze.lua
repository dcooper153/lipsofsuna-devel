-- Summon plagued beasts.
Feateffectspec:extend{
	name = "black haze",
	modifier = function(self, mod, secs)
		-- Wait five seconds.
		mod.strength = 10000
		mod.timer = mod.timer + secs
		if mod.timer < 5 then return true end
		mod.timer = mod.timer - 5
		-- Damage the actor.
		mod.object:damaged{amount = 5, type = "disease"}
		-- Infect nearby actors.
		local near = Object:find{point = mod.object.position, radius = 5}
		for k,v in pairs(near) do
			if math.random() > 0.1 then
				v:inflict_modifier("black haze", 10000)
			end
		end
		return true
	end,
	ranged = function(self, args)
		local s = Species:random{category = "plague"}
		if not s then return end
		local p = args.point + Vector(
			-1 + 2 * math.random(),
			-1 + 2 * math.random(),
			-1 + 2 * math.random())
		local o = Creature{spec = s, position = p, random = true, realized = true}
		o:inflict_modifier("black haze", 10000)
	end}
