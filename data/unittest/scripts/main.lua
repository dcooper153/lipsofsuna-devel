-- TODO: object_set_position doesn't work due to userdata recognition being broken.


-- TODO: Garbage collection doesn't work for tables, needs userdata hacks.
-- * Do we need the event at all? The userdata is collected automatically
--   and scripts can easily control its lifetime by keeping it in the table.
-- * Just remember to associate models to their objects.

local catch = function(f)
	local s,e = pcall(f)
	if not s then print(e) end
end

------------------------------------------------------------------------------

require "system/class"
catch(function() Class.unittest() end)

require "system/core"
catch(function() Program.unittest() end)

require "system/math"
catch(function() Vector.unittest() end)
catch(function() Quaternion.unittest() end)

require "system/model"
catch(function() Model.unittest() end)

require "system/object"
require "system/object-physics"
Physics.enable_simulation = true
catch(function() Object.unittest() end)

require "system/tiles"
catch(function() Material.unittest() end)

require "system/database"
catch(function() Database.unittest() end)

require "system/tiles"
catch(function() Voxel.unittest() end)

require "system/graphics"
-- TODO

require "system/widgets"
catch(function() Widget.unittest() end)

require "system/render"
catch(function() Light.unittest() end)
