--- Particle effect.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.particle
-- @alias Particle

local Class = require("system/class")

if not Los.program_load_extension("particle") then
	error("loading extension `particle' failed")
end

------------------------------------------------------------------------------

--- Particle effect.
-- @type Particle
local Particle = Class("Particle")

--- Creates a new particle set.
-- @param clss Particle class.
-- @return Particle.
Particle.new = function(clss, model)
	local self = Class.new(clss)
	self.handle = Los.particle_new()
	return self
end

--- Adds a particle effect from a model.
-- @param self Particle.
-- @param model Model.
Particle.add_model = function(self, model)
	Los.particle_add_model(self.handle, model.handle)
end

--- Adds a particle effect from a script.
-- @param self Particle.
-- @param name Script name.
Particle.add_script = function(self, name)
	Los.particle_add_ogre(self.handle, name)
end

--- Instantly clears all the particles.
-- @param self Particle.
Particle.clear = function(self)
	Los.particle_clear(self.handle)
end

--- Enables or disables particle emitting.
-- @param self Particle.
-- @param v True to enable. False otherwise.
Particle.set_emitting = function(self, v)
	Los.particle_set_emitting(self.handle, v)
end

--- Sets the position of the particle set.
-- @param self Particle.
-- @param v Vector.
Particle.set_position = function(self, v)
	Los.particle_set_position(self.handle, v.x, v.y, v.z)
end

--- Sets the render queue of the particle set.
-- @param self Particle.
-- @param name Queue name.
Particle.set_render_queue = function(self, name)
	Los.particle_set_render_queue(self.handle, name)
end

--- Sets the rotation of the particle set.
-- @param self Particle.
-- @param v Quaternion.
Particle.set_rotation = function(self, v)
	Los.particle_set_rotation(self.handle, v.x, v.y, v.z, v.w)
end

--- Sets the size of the particle set.
-- @param self Particle.
-- @param v True to make visible. False otherwise.
Particle.set_visible = function(self, v)
	Los.particle_set_visible(self.handle, v)
end

return Particle
