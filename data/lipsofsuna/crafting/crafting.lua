--- Crafting subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module crafting.crafting
-- @alias Crafting

local Class = require("system/class")
local Item = require("core/objects/item")
local Physics = require("system/physics")
local Obstacle = require("core/objects/obstacle")
local Player = require("core/objects/player")
local TerrainManager = require("core/terrain/terrain-manager")
local Vector = require("system/math/vector")

--- Crafting subgame.
-- @type Crafting
local Crafting = Class("Crafting")

--- Creates the crafting subgame.
-- @param clss Crafting class.
-- @return Crafting.
Crafting.new = function(clss)
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
	Main.terrain = self.terrain --FIXME
	-- Enable the simulation.
	Physics:set_enable_simulation(true)
	return self
end

--- Closes the subgame.
-- @param self Crafting.
Crafting.close = function(self)
	-- TODO
end

--- Updates the subgame state.
-- @param self Crafting.
-- @param secs Seconds since the last update.
Crafting.update = function(self, secs)
	-- Initialize the player.
	if not self.player then
		self.player = Player(Main.objects)
		self.player:set_spec(Actorspec:find_by_name("crafting player"))
		self.player:randomize()
		self.player.get_admin = function() return true end --FIXME
		self.player:set_position(Vector(500,101,500))
		self.player.physics:set_collision_group(Game.PHYSICS_GROUP_PLAYERS)
		self.player:set_visible(true)
		self.player:set_client(-1)
		Client:set_player_object(self.player)
		Server.players_by_client = {}
		Server.players_by_client[-1] = self.player --FIXME
		self.player:calculate_animation()

		local alchemy = Obstacle(Main.objects)
		alchemy:set_spec(Obstaclespec:find_by_name("alchemy table"))
		alchemy:set_position(Vector(495,100.1,500))
		alchemy:set_visible(true)

		local anvil = Obstacle(Main.objects)
		anvil:set_spec(Obstaclespec:find_by_name("anvil"))
		anvil:set_position(Vector(500,100.1,495))
		anvil:set_visible(true)

		local spelltable = Obstacle(Main.objects)
		spelltable:set_spec(Obstaclespec:find_by_name("spell table"))
		spelltable:set_position(Vector(500,100.1,505))
		spelltable:set_visible(true)

		local workbench = Obstacle(Main.objects)
		workbench:set_spec(Obstaclespec:find_by_name("workbench"))
		workbench:set_position(Vector(505,100.1,500))
		workbench:set_visible(true)

		local chest = Item(Main.objects)
		chest:set_spec(Itemspec:find_by_name("crafting chest"))
		chest:set_position(Vector(505,100.1,510))
		chest:set_visible(true)
	end
	-- Update lighting.
	Client.lighting:update(secs)
	-- Update terrain.
	self.terrain:refresh_chunks_by_point(Vector(500, 0, 500), 20)
end

return Crafting
