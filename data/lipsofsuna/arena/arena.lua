--- Arena subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module arena.arena
-- @alias Arena

local Class = require("system/class")
local Physics = require("system/physics")
local Player = require("core/objects/player")
local TerrainManager = require("core/terrain/terrain-manager")
local Vector = require("system/math/vector")

--- Arena subgame.
-- @type Arena
Arena = Class("Arena")

--- Creates the arena subgame.
-- @param clss Arena class.
-- @return Arena.
Arena.new = function(clss)
	local self = Class.new(clss)
	-- FIXME: Initialize the game.
	Main:start_game("benchmark")
	Main.messaging:set_transmit_mode(true, true)
	Game.sectors.unload_time = nil
	-- Initialize the terrain.
	self.terrain = TerrainManager(8, 0.75, nil, false, true, true)
	self.terrain:set_view_center(Vector(500, 0, 500))
	self.terrain.generate_hooks:register(0, function(self)
		local w = self.manager.chunk_size
		local t = self.manager.terrain
		for z = 0,w-1 do
			for x = 0,w-1 do
				t:add_stick_corners(self.x + x, self.z + z, 0, 0, 0, 0, 100, 100, 100, 100, 3)
			end
		end
		for x = self.x-1,self.x+w do
			for z = self.z-1,self.z+w do
				t:calculate_smooth_normals(x, z)
			end
		end
	end)
	Game.terrain = self.terrain --FIXME
	-- Enable the simulation.
	Physics:set_enable_simulation(true)
	return self
end

--- Closes the subgame.
-- @param self Arena.
Arena.close = function(self)
	-- TODO
end

--- Updates the subgame state.
-- @param self Arena.
-- @param secs Seconds since the last update.
Arena.update = function(self, secs)
	-- Initialize the player.
	if not self.player then
		self.player = Player{spec = Actorspec:find_by_name("aer-player")}
		self.player.has_server_data = function() return true end --FIXME
		self.player:set_position(Vector(500,101,500))
		self.player.physics:set_collision_group(Game.PHYSICS_GROUP_PLAYERS)
		self.player:set_visible(true)
		self.player.render:init(self.player)
		self.player:set_client(-1)
		Client:set_player_object(self.player)
		Server.players_by_client = {}
		Server.players_by_client[-1] = self.player --FIXME
		self.player:calculate_animation()
	end
	-- Update lighting.
	Client.lighting:update(secs)
	-- Update terrain.
	self.terrain:refresh_chunks_by_point(Vector(500, 0, 500), 20)
	self.terrain:update(secs)
end

return Arena
