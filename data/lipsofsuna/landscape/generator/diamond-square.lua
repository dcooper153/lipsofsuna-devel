--- Diamond-Square fractal terrain algorithm.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.diamond_square
-- @alias DiamondSquare

local Class = require("system/class")
local Noise = require("system/noise")
local NumberArray2d = require("system/array/number-2d")

--- Diamond-Square fractal terrain algorithm.
-- @type DiamondSquare
local DiamondSquare = Class("DiamondSquare")

--- Creates a new Diamond-Square terrain.
-- @param clss DiamondSquare class.
-- @param size Terrain size. Must be a power of two.
-- @return DiamondSquare.
DiamondSquare.new = function(clss, size)
	local self = Class.new(clss)
	self.__size = size
	self.__a = NumberArray2d(size+1, size+1)
	return self
end

--- Calculates the terrain heights.
-- @param self DiamondSquare.
-- @param seeds Array of seeds.
DiamondSquare.calculate = function(self, seeds)
	local b = 5000
	local e = 500
	local m = self.__size
	-- Height sampling function.
	local height = function(x, z, b, e)
		local f = 4 * Noise:plasma_noise_2d(seeds[2] + 0.1 * x, seeds[1] + 0.1 * z, 2)
		return b + f * e * Noise:plasma_noise_2d(seeds[1] + 0.01 * x, seeds[2] + 0.01 * z, 2)
	end
	-- Generate the corner vertices.
	local w = m
	local d = m / 2
	local a = self.__a
	a:set(0, 0, b)
	a:set(m, 0, b)
	a:set(0, m, b)
	a:set(m, m, b)
	-- Generate the other vertices.
	--
	-- P--d--P--d--P    P: Vertex from the previous level
	-- |  |  |  |  |    S: Vertex generates by square pass
	-- D--S--D--S--D    d: Vertex generates by diamond pass 1
	-- |  |  |  |  |    D: Vertex generates by diamond pass 2
	-- P--d--P--d--P
	while d >= 1 do
		-- Square.
		for z = d,m,w do
			for x = d,m,w do
				local h = a:get(x-d, z-d) + a:get(x+d, z-d) + a:get(x-d, z+d) + a:get(x+d, z+d)
				a:set(x, z, height(x, z, h/4, e))
			end
		end
		-- Diamond 1.
		for z = 0,m,w do
			for x = d,m,w do
				local n = 2
				local h = a:get(x-d, z) + a:get(x+d, z)
				if z > 0 then n,h = n+1, h+a:get(x, z-d) end
				if z < m-d then n,h = n+1, h+a:get(x, z+d) end
				a:set(x, z, height(x, z, h/n, e))
			end
		end
		-- Diamond 2.
		for z = d,m,w do
			for x = 0,m,w do
				local n = 2
				local h = a:get(x, z-d) + a:get(x, z+d)
				if x > 0 then n,h = n+1, h+a:get(x-d, z) end
				if x < m-d then n,h = n+1, h+a:get(x+d, z) end
				a:set(x, z, height(x, z, h/n, e))
			end
		end
		-- Next level.
		w = d
		d = d/2
		e = e/2
	end
end

--- Prints the heights into the terminal.
-- @param self DiamondSquare.
DiamondSquare.print = function(self)
	local m = self.__size
	for z = 0,m do
		local r = {}
		for x = 0,m do
			r[x+1] = string.format("%5d", self.__a:get(x, z))
		end
		print(table.concat(r, " "))
	end
end

--- Gets the bilinearly interpolated height at the given point.
-- @param self DiamondSquare.
-- @param x X floating point offset.
-- @param z Z floating point offset.
-- @return Height.
DiamondSquare.get_height = function(self, x, z)
	return self.__a:get_bilinear(x, z)
end

return DiamondSquare
