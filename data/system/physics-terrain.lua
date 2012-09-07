--- Collision detection for the stick-like terrain system.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.physics_terrain
-- @alias PhysicsTerrain

local Class = require("system/class")
local Model = require("system/model")
local Packet = require("system/packet")
local Vector = require("system/math/vector")

if not Los.program_load_extension("physics-terrain") then
	error("loading extension `physics-terrain' failed")
end

------------------------------------------------------------------------------

--- Collision detection for the stick-like terrain system.
-- @type PhysicsTerrain
PhysicsTerrain = Class("PhysicsTerrain")

--- Creates a new physics terrain.
-- @param clss PhysicsTerrain class.
-- @param terrain Terrain.
-- @return PhysicsTerrain.
PhysicsTerrain.new = function(clss, terrain)
	local self = Class.new(clss)
	self.handle = Los.physics_terrain_new(terrain.handle)
	return self
end

--- Returns true if the physics terrain has a terrain object assigned.</br>
--
-- This is mainly for unittesting purposes since normally the terrain
-- manager in the scripts keeps the object around. Regardless, the engine
-- must be able to handle sudden garbage collection of the terrain object.
--
-- @param self PhysicsTerrain.
-- @return Boolean.
PhysicsTerrain.get_valid = function(self)
	return Los.physics_terrain_get_valid(self.handle)
end

--- Gets the visibility of the physics terrain.
-- @param self PhysicsTerrain.
-- @return Boolean.
PhysicsTerrain.get_visible = function(self, v)
	return self.__visible
end

--- Sets the visibility of the physics terrain.
-- @param self PhysicsTerrain.
-- @param v Boolean.
PhysicsTerrain.set_visible = function(self, v)
	self.__visible = v
	Los.physics_terrain_set_visible(self.handle, v)
end

return PhysicsTerrain
