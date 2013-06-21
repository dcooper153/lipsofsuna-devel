--- Landscape generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.generator
-- @alias Generator

local Class = require("system/class")
local Material = require("system/material")
local Marker = require("core/marker")
local Network = require("system/network")
local Region = require("landscape/generator/region")
local Sector = require("system/sector")
local SectorGenerator = require("landscape/generator/sector-generator")
local Staticobject = require("core/objects/static")

--- Landscape generator.
-- @type Generator
Generator = Class("Generator")
Generator.map_size = Vector(1000, 1000, 1000)
Generator.map_start = Vector(600, 600, 600) - Generator.map_size * 0.5
Generator.map_end = Vector(600, 600, 600) + Generator.map_size * 0.5
Generator.map_version = "7"
Generator.sector_types = {}

--- Creates a new map generator.
-- @param clss Generator class.
-- @return Generator.
Generator.new = function(clss)
	local self = Class.new(Generator)
	-- Initialize random seeds.
	self.seed1 = math.random(10000, 60000)
	self.seed2 = math.random(10000, 60000)
	self.seed3 = math.random(10000, 60000)
	-- Reset the status.
	self:reset()
	return self
end

--- Generates the world map.
-- @param self Generator.
-- @param args Arguments.
Generator.generate = function(self, args)
	-- Remove all player characters.
	for k,v in pairs(Server.players_by_client) do
		v:detach(true)
	end
	Server.players_by_client = {}
	-- Reset the world.
	self:update_status(0, "Resetting world")
	Marker:reset()
	Game.sectors:unload_all()
	self:reset()
	-- Save the map.
	self:update_status(0, "Saving the map")
	Server.object_database:clear_objects()
	Game.sectors:save_world(true, function(p) self:update_status(p) end)
	Game.sectors:unload_all()
	Server.object_database:save_static_objects()
	Server.serialize:set_value("map_version", Generator.map_version)
	-- Save map markers.
	self:update_status(0, "Saving quests")
	Server.serialize:save_generator(true)
	Server.serialize:save_markers(true)
	Server.quest_database:reset()
	Server.account_database:save_accounts(true)
	Server.object_database:clear_world_decay()
	-- Discard events emitted during map generation so that they
	-- don't trigger when the game starts.
	self:update_status(0, "Finishing")
	Program:update()
	repeat until not Program:pop_event()
	Program:update()
	repeat until not Program:pop_event()
	-- Inform players of the generation being complete.
	-- All accounts were erased so clients need to re-authenticate.
	for k,v in pairs(Network:get_clients()) do
		Main.messaging:server_event("login", v)
	end
end

--- Informs clients of the generator status.
-- @param self Generator.
-- @param client Specific client to inform or nil to inform all.
-- @return Network packet.
Generator.inform_clients = function(self, client)
	local msg = self.prev_message or ""
	local prg = self.prev_fraction or 0
	if client then
		Main.messaging:server_event("generator status", client, msg, prg)
	else
		for k,v in pairs(Network:get_clients()) do
			Main.messaging:server_event("generator status", v, msg, prg)
		end
	end
end

Generator.reset = function(self)
	self.sectors = {}
end

--- Updates the network status while the generator is active.
-- @param self Generator.
Generator.update_network = function(self)
	Network:update()
	while true do
		local event = Program:pop_event()
		if not event then break end
		if event.type == "login" then
			self:inform_clients(event.client)
		end
	end
end

--- Updates the status message of the generator.
-- @param self Generator.
-- @param frac Fraction of the task completed.
-- @param msg Message string.
Generator.update_status = function(self, frac, msg)
	if msg then
		print(math.ceil(frac * 100) .. "% " .. msg)
		self.prev_message = msg
		self.prev_fraction = frac
		self:inform_clients()
		self:update_network()
	elseif frac == 1 or frac > self.prev_fraction + 0.05 then
		print(math.ceil(frac * 100) .. "% " .. self.prev_message)
		self.prev_fraction = frac
		self:inform_clients()
		self:update_network()
	end
end

------------------------------------------------------------------------------
-- TODO: Move to utils

Generator.get_sector_type_by_id = function(self, id)
	return self.sectors[id]
end

Generator.set_sector_type_by_id = function(self, id, type)
	self.sectors[id] = type
end

Generator.set_sector_type_by_offset = function(self, offset, type)
	self:set_sector_type_by_id(Sector:get_id_by_offset(offset), type)
end

Generator.is_overworld_sector_by_id = function(self, id)
	local tile = Sector:get_tile_by_id(id)
	return tile.y > 1000
end

Generator.is_overworld_sector_by_point = function(self, point)
	local tile_y = point.y
	return tile_y > 1000
end


