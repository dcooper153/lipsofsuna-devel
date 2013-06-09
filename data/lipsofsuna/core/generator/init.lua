local File = require("system/file")

for k,v in pairs(File:scan_directory(Mod.path .. "sectors")) do
	require(Mod.path .. "sectors/" .. string.gsub(v, "([^.]*).*", "%1"))
end

local generate = function(self)
	-- FIXME: Call the generator instead.
	local MapUtils = require("core/server/map-utils")
	local Noise = require("system/noise")
	local get_surface_height_at = function(x, z)
		-- Choose the bumpiness of the region.
		-- p=0.7: Very smooth.
		-- p=0.9: Very bumpy.
		local r = Noise:plasma_noise_2d(0.01 * x, 0.01 * z, 2)
		local p = 0.75 + 0.15 * r
		-- Choose the height of the region.
		-- This is affected by both the position and the bumpiness.
		local n1 = Noise:harmonic_noise_2d(0.001 * x, 0.001 * z, 6, 1.3, p)
		-- Choose the soil layer height.
		-- This is affected by the height and the bumpiness.
		local s_base = 0.3 - 0.7 * (r + n1)
		local s_rand = Noise:harmonic_noise_2d(412 + 0.02 * x, 903 + 0.02 * z, 3, 1.3, 1 - 0.25 * (n1 + r))
		local n2 = math.max(0, 0.5 * (s_base + s_rand))
		-- Choose the grass layer height.
		-- This is mostly just random on any areas with soil.
		local g = Noise:plasma_noise_2d(0.03 * x, 0.03 * z, 3 - r)
		local n3 = math.max(0, g) * n2
		-- Return the combined heights.
		return 100 + 100 * n1, 2 * n2, 0.5 * n3
	end
	local get_dungeon_height_at = function(x, z)
		-- Choose the dungeon amount of the region.
		local r = Noise:plasma_noise_2d(0.001 * x, 0.001 * z, 3)
		if r < 0 then return 0, 0 end
		-- Choose the roughness of the dungeon.
		local r = Noise:plasma_noise_2d(0.01 * x, 0.01 * z, 2)
		local p = 0.5 + 0.35 * r
		-- Choose the vertical offset.
		local y1 = math.abs(Noise:harmonic_noise_2d(0.001 * x, 0.001 * z, 6, 1.3, p))
		local y2 = math.abs(Noise:harmonic_noise_2d(0.0001 * x, 0.0001 * z, 6, 1.3, 0.5))
		-- Choose the height.
		local h = math.abs(Noise:plasma_noise_2d(0.035 * x, 0.035 * z, 2))
		if h < 0.15 then
			h = 0
		end
		-- Return the combined heights,
		return 80 + 70 * y1 - 70 * y2, h * 10
	end
	local w = self.manager.chunk_size
	local t = self.manager.terrain
	-- Calculate the heights.
	local h = {}
	local i = 0
	for z = 0,w do
		for x = 0,w do
			h[i], h[i + 1], h[i + 2] = get_surface_height_at(self.x + x, self.z + z)
			h[i + 3], h[i + 4] = get_dungeon_height_at(self.x + x, self.z + z)
			i = i + 5
		end
	end
	-- Generate the surface.
	local p = Vector()
	local stride = 5 * (w + 1)
	local get_height_at = function(x, z)
		local i = 5 * x + stride * z
		return h[i], h[i + 1], h[i + 2]
	end
	local get_dungeon_at = function(x, z)
		local i = 5 * x + stride * z
		local y, h = h[i + 3], h[i + 4]
		--return y - h*0.1, y + h*0.9
		return y - h/2, y + h/2
	end
	for z = 0,w-1 do
		for x = 0,w-1 do
			-- Calculate the corner heights.
			local a00,b00,c00 = get_height_at(x, z)
			local a10,b10,c10 = get_height_at(x + 1, z)
			local a01,b01,c01 = get_height_at(x, z + 1)
			local a11,b11,c11 = get_height_at(x + 1, z + 1)
			-- Calculate the dungeon corner heights.
			local d00,e00 = get_dungeon_at(x, z)
			local d10,e10 = get_dungeon_at(x + 1, z)
			local d01,e01 = get_dungeon_at(x, z + 1)
			local d11,e11 = get_dungeon_at(x + 1, z + 1)
			-- Generate the grass.
			if c00 > 0 or c10 > 0 or c01 > 0 or c11 > 0 then
				local y0,y1,y2,y3 = a00 + b00 + c00, a10 + b10 + c10, a01 + b01 + c01, a11 + b11 + c11
				t:add_stick_corners(self.x + x, self.z + z, 0, 0, 0, 0, y0, y1, y2, y3, 2)
				-- Generate plants.
				if e00 < y0 and e10 < y1 and e01 < y2 and e11 < y3 then
					local r = math.random() - 0.99
					if r >= 0 then
						-- Calculate the position.
						p:set_xyz(self.x + x + 0.5, 0.0, self.z + z + 0.5)
						p:multiply(self.manager.grid_size)
						p:add_xyz(0, (y0 + y1 + y2 + y3) / 4, 0)
						-- Calculate the forest ratio.
						local f = Noise:plasma_noise_2d(2342 + 0.005 * x, 593 + 0.005 * z, 2)
						-- Choose and create the obstacle.
						if r > f * 0.01 then
							MapUtils:place_obstacle{point = p, category = "tree"}
						else
							MapUtils:place_obstacle{point = p, category = "small-plant"}
						end
					end
				end
			end
			-- Generate the soil.
			if b00 > 0 or b10 > 0 or b01 > 0 or b11 > 0 then
				t:add_stick_corners(self.x + x, self.z + z, 0, 0, 0, 0, a00 + b00, a10 + b10, a01 + b01, a11 + b11, 1)
			end
			-- Generate the stone.
			t:add_stick_corners(self.x + x, self.z + z, 0, 0, 0, 0, a00, a10, a01, a11, 3)
			-- Generate the dungeon.
			if d00 < e00 and d10 < e10 and d01 < e01 and d11 < e11 then
				t:add_stick_corners(self.x + x, self.z + z, d00, d10, d01, d11, e00, e10, e01, e11, 0)
			end
		end
	end
	-- Smoothen the surface.
	for x = self.x-1,self.x+w do
		for z = self.z-1,self.z+w do
			t:calculate_smooth_normals(x, z)
		end
	end
end

Main.game_start_hooks:register(5, function()
	if Main.game.terrain then
		Main.game.terrain.generate_hooks:register(20, generate)
	end
end)
