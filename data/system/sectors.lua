--- Provides access to the sector manager of the engine.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.sectors
-- @alias Sectors

if not Los.program_load_extension("sectors") then
	error("loading extension `sectors' failed")
end

------------------------------------------------------------------------------

local Class = require("system/class")

--- Provides access to the sector manager of the engine.
-- @type Sectors
Sectors = Class("Sectors")

--- Increases the timestamp of the sectors inside the given sphere.<br/>
--
-- This is useful when the timestamp is used for unloading sectors,
-- since you can easily refresh active sectors.
--
-- @param clss Sectors class.
-- @param point Position vector, in world units.
-- @param radius Refresh radius, in world units.
Sectors.refresh = function(self, point, radius)
	Los.sectors_refresh(point.handle, radius)
end

--- Unloads all sectors.<br/>
--
-- Unrealizes all normal objects in all sectors and clears the terrain.
-- The sector list is completely cleared after this.
--
-- @param clss Sectors class.
Sectors.unload_all = function(clss)
	Los.sectors_unload_all()
end

--- Unloads a sector.<br/>
--
-- Unrealizes all normal objects in the sector and clears the terrain in the sector.
-- The sector is removed from the sector list after this.
--
-- @param clss Sectors class.
-- @param id Sector ID.
Sectors.unload_sector = function(clss, id)
	Los.sectors_unload_sector(id)
end

--- Gets the dictionary of active sector IDs.
-- @param clss Sectors class.
-- @return Dictionary of number keys and boolean values.
Sectors.get_sectors = function(clss)
	return Los.sectors_get_sectors()
end

--- Gets the idle time of a sector.
-- @param clss Sectors class.
-- @param id Sector ID.
-- @return Age in seconds, or nil.
Sectors.get_sector_idle = function(clss, id)
	return Los.sectors_get_sector_idle(id)
end

--- Gets the sector size in world units.
-- @param clss Sectors class.
-- @return Number.
Sectors.get_sector_size = function(clss)
	return Los.sectors_get_sector_size()
end

return Sectors
