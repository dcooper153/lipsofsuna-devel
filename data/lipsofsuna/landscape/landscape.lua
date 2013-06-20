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
	-- FIXME: Initialize the game.
	Main:start_game("benchmark")
	Game.sectors.unload_time = nil
	-- Initialize the terrain.
	self.terrain = TerrainManager(8, 0.75, nil, false, true, true)
	self.terrain:set_view_center(Vector(500, 0, 500))
	self.terrain.generate_hooks:register(0, TerrainGenerator.generate)
	return self
end

--- Closes the subgame.
-- @param self Landscape.
Landscape.close = function(self)
	-- TODO
end

--- Updates the subgame state.
-- @param self Landscape.
-- @param secs Seconds since the last update.
Landscape.update = function(self, secs)
	-- Update lighting.
	Client.lighting:update(secs)
	-- Update terrain.
	self.terrain:refresh_chunks_by_point(Vector(500, 0, 500), 50)
	self.terrain:update(secs)
end

return Landscape
