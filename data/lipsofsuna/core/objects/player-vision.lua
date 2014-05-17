--- Player vision updater.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.player_vision
-- @alias PlayerVision

local Class = require("system/class")
local Packet = require("system/packet")
local Vision = require("system/vision")

--- Player vision updater.
-- @type PlayerVision
local PlayerVision = Class("PlayerVision", Vision)

--- Creates a new player object.
-- @param clss PlayerVision class.
-- @param player Player.
-- @return PlayerVision.
PlayerVision.new = function(clss, player)
	local self = Vision.new(clss, player:get_id(), player.manager)
	self:set_cone_factor(0.5)
	self:set_cone_angle(math.pi/2.5)
	self:set_enabled(true)
	self.object = player
	self.callback = function(args) self:__callback(args) end
	self.__timer = 0
	self.__terrain = {}
	return self
end

--- Updates the vision state.
-- @param self PlayerVision.
-- @param secs Seconds since the last update.
PlayerVision.update = function(self, secs)
	if not self.object:get_visible() then return end
	self.__timer = self.__timer + secs
	if self.__timer > 0.05 then
		self:__update_radius()
		Vision.update(self, self.__timer)
		self.__timer = 0
		self:__update_markers()
		self:__update_terrain()
	end
end

--- Updates the vision radius.
-- @param self PlayerVision.
PlayerVision.__update_radius = function(self)
	local radius = 50
	local pos = self.object:get_position():copy()
	local rot = self.object:get_rotation()
	self:set_direction(Vector(0,0,-1):transform(rot))
	self:set_position(pos)
	self:set_radius(radius)
end

--- Updates the map markers.
-- @param self PlayerVision.
PlayerVision.__update_markers = function(self)
	local pos = self.object:get_position()
	local radius = 3 * self:get_radius()
	for k,v in pairs(Main.markers.__dict_discoverable) do
		if (pos - v.position).length < radius then
			v:unlock()
		end
	end
end

--- Updates the terrain.
-- @param self PlayerVision.
PlayerVision.__update_terrain = function(self)
	-- Check if terrain vision is needed.
	if not Main.game.enable_generation then return end
	local client = self.object.client
	if client == -1 then return end
	-- Get the nearby terrain chunks.
	local pos = self.object:get_position()
	local radius = self:get_radius()
	local chunks = Main.terrain.terrain:get_chunks_in_circle(pos, radius)
	-- Synchronize the outdated chunks.
	local msgid = Main.messaging:get_event_id("update terrain")
	for x,z,stamp1 in chunks do
		if stamp1 then
			local id = Main.terrain:get_chunk_id_by_xz(x, z)
			local stamp0 = self.__terrain[id]
			if not stamp0 or stamp0 ~= stamp1 then
				local packet = Packet(msgid, "uint32", x, "uint32", z)
				Main.terrain.terrain:get_chunk_data(x, z, packet)
				Main.messaging:server_event("update terrain", client, packet)
				self.__terrain[id] = stamp1
			end
		end
	end
end

--- Handles a vision event.
-- @param self PlayerVision.
-- @param args Event arguments.
PlayerVision.__callback = function(self, args)
	local player = self.object
	local client = player.client
	local funs
	funs =
	{
		["object-animated"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object animated", client, o:get_id(), args.animation or "", args.time or 0.0, args.variant)
		end,
		["object attack"] = function(args)
			local o = args.object
			Main.messaging:server_event("object attack", client, o:get_id(), args.move, args.variant)
		end,
		["object-beheaded"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object beheaded", client, o:get_id(), o:get_beheaded())
		end,
		["object-combat"] = function(args)
			local o = args.object
			Main.messaging:server_event("object combat", client, o:get_id(), o:get_combat_hint())
		end,
		["object-dead"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object dead", client, o:get_id(), args.dead)
		end,
		["object-dialog"] = function(args)
			local o = args.object
			local mine = (o.dialog and o.dialog.user == player or false)
			if args.choices then
				Main.messaging:server_event("object dialog choice", client, o:get_id(), mine, args.character, args.message, args.choices)
			elseif args.message then
				Main.messaging:server_event("object dialog say", client, o:get_id(), mine, args.character or "", args.message)
			else
				Main.messaging:server_event("object dialog none", client, o:get_id())
			end
		end,
		["object-effect"] = function(args)
			local o = args.object
			Main.messaging:server_event("object effect", client, o:get_id(), args.effect)
		end,
		["object-hidden"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object hidden", client, o:get_id())
		end,
		["object-motion"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object moved", client, o:get_id(), o:get_position(), o:get_rotation(), o:get_tilt_angle(), o:get_velocity())
		end,
		["object-shown"] = function(args)
			-- Don't send static objects.
			local o = args.object
			if o:get_static() then return end
			-- Notify the client.
			Main.messaging:server_event("object shown", client, o == player, o)
			-- Wake up the AI.
			if o.ai then o.ai:refresh() end
		end,
		["object-speech"] = function(args)
			local o = args.object
			Main.messaging:server_event("object speech", client, o:get_id(), args.message)
		end,
		["stat changed"] = function(args)
			local o = args.object
			local s = o.stats
			if not s then return end
			local v = s:get_skill(args.name)
			if not v then return end
			if v.prot == "public" or player == o then
				Main.messaging:server_event("object stat", client, o:get_id(), args.name, args.value, args.maximum, math.ceil(args.value - args.value_prev + 0.5))
			end
		end,
		["object-equip"] = function(args)
			-- If the player is subscribed to the inventory of the object, the
			-- equip message is already sent by the inventory listener.
			local id = args.object:get_id()
			if args.object.inventory:is_subscribed(player) then return end
			-- The contents of the inventory slot must be revealed to the client
			-- since it would otherwise have no information on the equipped item.
			Main.messaging:server_event("add inventory item", client, id, args.index, args.item.spec.name, args.item:get_count())
			-- Send the equip message.
			Main.messaging:server_event("equip inventory item", client, id, args.index, args.slot)
		end,
		["object-unequip"] = function(args)
			-- If the player is subscribed to the inventory of the object, the
			-- unequip message is already sent by the inventory listener.
			local id = args.object:get_id()
			if args.object.inventory:is_subscribed(player) then return end
			-- Send the unequip message.
			Main.messaging:server_event("unequip inventory item", client, id, args.index)
			-- The client doesn't need the item information of the unsubscribed
			-- inventory anymore so we can clear the item.
			Main.messaging:server_event("remove inventory item", client, id, args.index)
		end,
		["world-effect"] = function(args)
			Main.messaging:server_event("world effect", client, args.point, args.effect)
		end
	}
	local fun = funs[args.type]
	if fun then fun(args) end
end

return PlayerVision
