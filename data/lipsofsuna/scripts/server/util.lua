-- FIXME: These need to be cleaned up.

Actions = Class()
Actions.register = function(self, args)
	self[args.name] = args.func
end

Config = {}
Config.center = Vector(2048, 2048, 2048)
Config.spawn = Vector(2048, 2035, 2048)
Config.tilewidth = 4
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
		local m = Voxel:find_material{id = t.terrain}
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
	local t,p = Voxel:find_tile{point = point, match = "full"}
	if p then
		damage = 86
		if t.damage + damage > 255 then

			local m = Voxel:find_material{id = t.terrain}
			if m then
				if m.name == "crystal-000" then
					t.damage = 0
					t.terrain = t.terrain + 1
					Voxel:set_tile{point = p, tile = t}
					--Effect("shatter-000", c)
					return
				end
				if m.name == "crystal-001" then
					t.terrain = 0
					Voxel:set_tile{point = p, tile = t}
					return
				end
			end

			t.terrain = 0
			Voxel:set_tile{point = p, tile = t}
			--Effect("crack-000", result.point)
			if math.random(10) == 1 then
				Item:create{name = "gem", position = c, realized = true}
			end

		else
			t.damage = t.damage + damage
			Voxel:set_tile{point = p, tile = t}
		end
	end
end
