-- FIXME: These need to be cleaned up.

Actions = Class()
Actions.register = function(self, args)
	self[args.name] = args.func
end

Config = {}
Config.center = Vector(1024, 1024, 1024)
Config.spawn = Vector(1026, 1018, 1024)

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
	--local t = Voxel:get_tile{point = point}
	--local m = Material:find{id = t.terrain}
	Voxel:set_tile{point = point, tile = 0}
	--Effect("crack-000", result.point)
	if math.random(10) == 1 then
		local offset = Vector(math.random(), math.random(), math.random())
		Item:create{name = "gem", position = (point + offset) * Config.tilewidth, realized = true}
	end
end
