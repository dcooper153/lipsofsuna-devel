--- Landscape subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.landscape
-- @alias Landscape

local Class = require("system/class")
local TerrainGenerator = require("landscape/generator/terrain-generator")
local TerrainManager = require("core/terrain/terrain-manager")
local Vector = require("system/math/vector")

--- Landscape subgame.
-- @type Landscape
Landscape = Class("Landscape")

--- Creates the landscape subgame.
-- @param clss Landscape class.
-- @return Landscape.
Landscape.new = function(clss)
	local self = Class.new(clss)
	-- Initialize the terrain.
	self.terrain = TerrainManager(12, 1, nil, false, true, true)
	self.terrain.generate_hooks:register(0, TerrainGenerator.generate)
	Main.terrain = self.terrain --FIXME
	return self
end

--- Closes the subgame.
-- @param self Landscape.
Landscape.close = function(self)
	Main.terrain:unload_all()
	Main.terrain = nil
end

--- Updates the subgame state.
-- @param self Landscape.
-- @param secs Seconds since the last update.
Landscape.update = function(self, secs)
	-- Update lighting.
	Client.lighting:update(secs)
	-- Update terrain.
	local camera = Main.client.camera_manager:find_camera_by_type("landscape")
	if not camera then return end
	local center = camera:get_target_position()
	self.terrain:set_view_center(center)
	self.terrain:refresh_chunks_by_point(center, 50)
end

return Landscape
