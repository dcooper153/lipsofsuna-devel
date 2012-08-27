--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.wireframe
-- @alias Wireframe

local Class = require("system/class")

if not Los.program_load_extension("wireframe") then
	error("loading extension `wireframe' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Wireframe
local Wireframe = Class("Wireframe")

Wireframe.on = function(self)
	return Los.Wireframe_on()
end

return Wireframe


