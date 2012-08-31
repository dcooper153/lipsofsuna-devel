--- Stick-like terrain system.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.terrain
-- @alias Terrain

local Class = require("system/class")
local Packet = require("system/packet")
local Sector = require("system/sector")

if not Los.program_load_extension("terrain") then
	error("loading extension `terrain' failed")
end

------------------------------------------------------------------------------

--- Stick-like terrain system.
-- @type Terrain
Terrain = Class("Terrain")

Terrain.test = function(clss)
	-- TODO
	Los.terrain_test()
end

return Terrain
