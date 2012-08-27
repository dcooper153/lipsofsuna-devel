--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.reload
-- @alias Reload

local Class = require("system/class")

if not Los.program_load_extension("reload") then
	error("loading extension `reload' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Reload
local Reload = Class("Reload")

Reload.get_enabled = function(self)
	return Los.reload_get_enabled()
end

Reload.set_enabled = function(self, v)
	Los.reload_set_enabled(v)
end

return Reload


