-- FIXME: These need to be cleaned up.

Actions = Class()
Actions.register = function(self, args)
	self[args.name] = args.func
end

Config = {}
Config.center = Vector(1024, 1024, 1024)
Config.spawn = Vector(1024, 1026, 1024)

Config.tilewidth = 32 / Voxel.tiles_per_line
Config.tilescale = 1 / Config.tilewidth
Config.gravity = Vector(0, -20, 0)
Config.skillregen = 0.5

Skills.get_total = function(self)
	local n = self:get_names()
	local t = 0
	for k,v in pairs(n) do
		t = t + self:get_maximum{skill = v}
	end
	return t
end

Damage = {}
Damage.impulse = function(self, value)
	return 0.03 * value
end

makeloot = function(position)
	local loot = Item:create{category = "weapon"}
	loot.position = position + Vector(0.0, 1.0 + 0.3, 0.0)
	loot.realized = true
	return loot
end

makemeat = function(position)
	for i=1,10 do
		local p = Vector(math.random(100) - 50, math.random(100) - 50, math.random(100) - 50)
		local v = Vector(math.random(100) - 50, math.random(50), math.random(100) - 50)
		p = v * 0.01
		v = v * 0.1
		local a = {"meat-000", "meat-001"}
		local m = a[math.random(2)]
		local o = Decayer{model = m, position = position + p, velocity = v, realized = true}
		if i == 1 then
			o:effect{effect = "squish-000"}
			Particles:create(position, "squish")
		end
	end
end

--- Handles interaction between players and terrain.
-- @param self Voxels class.
-- @param player Player object.
-- @param point Contact point.
Voxel.player_contact = function(self, player, point)
	local resist = 30
	while true do
		local t,c = Voxel:find_voxel{point = point, match = "full"}
		if not c or (c - point).length > 3 then break end
		local m = Material:find{id = t.terrain}
		if not m then break end
		if m.name == "trapdoor-000" then
			Voxel:erase{point = c}
		else
			if m.name == "spikes-000" then
				resist = 0
			end
			break
		end
	end
	return resist
end

Voxel.damage = function(self, point, damage)
	local t = Voxel:get_tile{point = point}
	damage = 86
	if t.damage + damage > 255 then

		local m = Material:find{id = t.terrain}
		if m then
			if m.name == "crystal-000" then
				t.damage = 0
				t.terrain = t.terrain + 1
				Voxel:set_tile{point = point, tile = t}
				--Effect("shatter-000", c)
				return
			end
			if m.name == "crystal-001" then
				t.terrain = 0
				Voxel:set_tile{point = point, tile = t}
				return
			end
		end

		t.terrain = 0
		Voxel:set_tile{point = point, tile = t}
		--Effect("crack-000", result.point)
		if math.random(10) == 1 then
			local offset = Vector(math.random(), math.random(), math.random())
			Item:create{name = "gem", position = (point + offset) * Config.tilewidth, realized = true}
		end

	else
		t.damage = t.damage + damage
		Voxel:set_tile{point = point, tile = t}
	end
end
