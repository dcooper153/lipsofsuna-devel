--- 2D array of numbers.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.array.number_2d
-- @alias NumberArray2d

local Class = require("system/class")

if not Los.program_load_extension("array") then
	error("loading extension `array' failed")
end

------------------------------------------------------------------------------

--- 2D array of numbers.
-- @type NumberArray2d
local NumberArray2d = Class("NumberArray2d")

--- Creates a new array.
-- @param clss NumberArray2d class.
-- @param w Width.
-- @param h Height.
-- @return NumberArray2d.
NumberArray2d.new = function(clss, w, h)
	local self = Class.new(clss)
	self.w = w
	self.h = h
	self.handle = Los.number_array_2d_new(w, h)
	if not self.handle then error("out of memory") end
	return self
end

--- Gets a value.
-- @param self NumberArray2d.
-- @param x X index.
-- @param y Y index.
-- @return Number.
NumberArray2d.get = function(self, x, y)
	return Los.number_array_2d_get(self.handle, x, y)
end

--- Sets a value.
-- @param self NumberArray2d.
-- @param x X index.
-- @param y Y index.
-- @param value Number.
NumberArray2d.set = function(self, x, y, value)
	Los.number_array_2d_set(self.handle, x, y, value)
end

--- Gets a bilinearly interpolated value.
-- @param self NumberArray2d.
-- @param x X floating point offset.
-- @param y Y floating point offset.
-- @return Number.
NumberArray2d.get_bilinear = function(self, x, y)
	return Los.number_array_2d_get_bilinear(self.handle, x, y)
end

--- Writes the contents of the array to a packet.
-- @param self Terrain.
-- @param packet Packet. Nil to create a new one.
-- @return Packet on success. Nil otherwise.
NumberArray2d.get_data = function(self, packet)
	if not packet then
		local Packet = require("system/packet")
		packet = Packet(1)
		return Los.number_array_2d_get_data(self.handle, packet.handle) and packet
	else
		return Los.number_array_2d_get_data(self.handle, packet.handle) and packet
	end
	return p
end

--- Reads the contents of the array from a packet.
-- @param self Terrain.
-- @param packet Packet.
-- @return True on success. False otherwise.
NumberArray2d.set_data = function(self, packet)
	return Los.number_array_2d_set_data(self.handle, packet.handle)
end

--- Gets the gradient at the given point.
-- @param self NumberArray2d.
-- @param x X index.
-- @param y Y index.
-- @return Horizontal, vertical, magnitude and angle.
NumberArray2d.get_gradient = function(self, x, y)
	return Los.number_array_2d_get_gradient(self.handle, x, y)
end

--- Gets a bilinearly interpolated gradient.
-- @param self NumberArray2d.
-- @param x X floating point offset.
-- @param y Y floating point offset.
-- @return Horizontal, vertical, magnitude and angle.
NumberArray2d.get_gradient_bilinear = function(self, x, y)
	return Los.number_array_2d_get_gradient_bilinear(self.handle, x, y)
end

return NumberArray2d
