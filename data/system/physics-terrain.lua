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
local PhysicsTerrain = Class("PhysicsTerrain")

--- Creates a new physics terrain.
-- @param clss PhysicsTerrain class.
-- @param terrain Terrain.
-- @return PhysicsTerrain.
PhysicsTerrain.new = function(clss, terrain)
	local self = Class.new(clss)
	self.handle = Los.physics_terrain_new(terrain.handle)
	self.__id = 1
	self.__collision_group = 1
	self.__collision_mask = 0xFFFF
	return self
end

--- Gets the collision group of the physics terrain.
-- @param self PhysicsTerrain.
-- @return Number.
PhysicsTerrain.get_collision_group = function(self, v)
	return self.__collision_group
end

--- Sets the collision group of the physics terrain.
-- @param self PhysicsTerrain.
-- @param v Number.
PhysicsTerrain.set_collision_group = function(self, v)
	self.__collision_group = v
	Los.physics_terrain_set_collision_group(self.handle, v)
end

--- Gets the collision mask of the physics terrain.
-- @param self PhysicsTerrain.
-- @return Number.
PhysicsTerrain.get_collision_mask = function(self, v)
	return self.__collision_mask
end

--- Sets the collision mask of the physics terrain.
-- @param self PhysicsTerrain.
-- @param v Number.
PhysicsTerrain.set_collision_mask = function(self, v)
	self.__collision_mask = v
	Los.physics_terrain_set_collision_mask(self.handle, v)
end

--- Gets the friction coefficient of the physics terrain.
-- @param self PhysicsTerrain.
-- @return Number.
PhysicsTerrain.get_friction = function(self)
	return self.__friction or 1.0
end

--- Sets the friction coefficient of the physics terrain.
-- @param self PhysicsTerrain.
-- @param v Number.
PhysicsTerrain.set_friction = function(self, v)
	self.__friction = v
	Los.physics_terrain_set_friction(self.handle, v)
end

--- Gets the unique ID of the physics terrain.
-- @param self PhysicsTerrain.
-- @return Number.
PhysicsTerrain.get_id = function(self, v)
	return self.__id
end

--- Sets the unique ID of the physics terrain.
-- @param self PhysicsTerrain.
-- @param v Number.
PhysicsTerrain.set_id = function(self, v)
	self.__id = v
	Los.physics_terrain_set_id(self.handle, v)
end

--- Sets whether unloaded chunks collide with objects.
-- @param self PhysicsTerrain.
-- @param v Boolean.
PhysicsTerrain.set_unloaded_collision = function(self, v)
	Los.physics_terrain_set_unloaded_collision(self.handle, v)
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
