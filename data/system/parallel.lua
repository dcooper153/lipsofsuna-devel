--- Parallel processing.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.parallel
-- @alias Parallel

local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("parallel") then
	error("loading extension `parallel' failed")
end

------------------------------------------------------------------------------

--- Parallel processing.
-- @type Parallel
local Parallel = Class("Parallel")

--- Updates physics and renders the scene in parallel.
-- @param self Parallel class.
-- @param secs Update timestep.
Parallel.update_physics_and_render = function(self, secs)
	Los.parallel_update_physics_and_render(secs)
end

return Parallel
