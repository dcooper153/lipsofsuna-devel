--- A class for helping to debug physics.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.physics_debug
-- @alias PhysicsDebug

local Class = require("system/class")

if not Los.program_load_extension("physics-debug") then
	error("loading extension `physics-debug' failed")
end

------------------------------------------------------------------------------

--- A class for helping to debug physics.
--
-- @type PhysicsDebug
local PhysicsDebug = Class("PhysicsDebug")

--- Instantiates a new physics debug.
-- @param clss PhysicsDebug class.
PhysicsDebug.new = function(clss)
	local self = Class.new(clss)
	self.handle = Los.physics_debug_new()
	return self
end

--- Draws the physics debug visuals.
-- @param self PhysicsDebug.
PhysicsDebug.draw = function(self)
	Los.physics_debug_draw(self.handle)
end


return PhysicsDebug
