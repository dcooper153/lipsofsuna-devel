local Class = require("system/class")

if not Los.program_load_extension("reload") then
	error("loading extension `reload' failed")
end

------------------------------------------------------------------------------

local Reload = Class("Reload")

Reload.get_enabled = function(self)
	return Los.reload_get_enabled()
end

Reload.set_enabled = function(self, v)
	Los.reload_set_enabled(v)
end

return Reload
