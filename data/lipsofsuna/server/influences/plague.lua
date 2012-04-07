-- Summon plagued beasts.
Feateffectspec:extend{
	name = "black haze",
	modifier = function(self, object, args, secs)
		-- Wait five seconds.
		object.plague_timer = (object.plague_timer or 0) + secs
		if object.plague_timer < 5 then return {st=10000} end
		object.plague_timer = object.plague_timer - 5
		-- Damage the actor.
		object:damaged{amount = 5, type = "disease"}
		-- Infect nearby actors.
		local near = Object:find{point = object.position, radius = 5}
		for k,v in pairs(near) do
			if math.random() > 0.1 then
				v:inflict_modifier("black haze", 10000)
			end
		end
		return {st=10000}
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
