--- Player object.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.player
-- @alias Player

local Actor = require("core/objects/actor")
local Class = require("system/class")
local Modifier = require("core/server/modifier")
local ModifierSpec = require("core/specs/modifier")
local Packet = require("system/packet")
local PlayerVision = require("core/objects/player-vision")

--- Player object.
-- @type Player
local Player = Class("Player", Actor)

--- Creates a new player object.
-- @param clss Player class.
-- @param manager Object manager.
-- @param id Unique object ID. Nil for a random free one.
-- @return Player.
Player.new = function(clss, manager, id)
	local self = Actor.new(clss, manager, id)
	self.inventory_subscriptions = {}
	self.running = true
	self:calculate_speed()
	return self
end

--- Adds a modifier to the object.
-- @param self Object.
-- @param modifier Modifier.
Player.add_modifier = function(self, modifier)
	Main.messaging:server_event("add modifier", self.client, modifier.name, modifier.strength or -1)
	return Actor.add_modifier(self, modifier)
end

--- Causes the player to die and respawn.
-- @param self Player.
Player.die = function(self)
	if Actor.die(self) then
		self:send_message("You have died...")
	else
		self:send_message("Saved by Sanctuary.")
	end
end

Player.disable = function(self, keep)
	if not Server.initialized then return end
	if self.vision then
		self.vision:set_enabled(false)
		self.vision = nil
	end
	if not keep and self.client then
		Main.messaging:server_event("start character creation", self.client)
		Server.players_by_client[self.client] = nil
		self.client = nil
	end
	Server.events:notify_action("player disable", self)
end

Player.detach = function(self, keep)
	self:disable()
	Actor.detach(self)
end

Player.handle_inventory_event = function(self, args)
	local id = args.inventory.id
	local funs =
	{
		["inventory-changed"] = function()
			if args.object then
				local name = args.object.spec.name
				local slot = args.inventory:get_slot_by_index(args.index)
				Main.messaging:server_event("add inventory item", self.client, id, args.index, name, args.object:get_count())
				if slot then
					Main.messaging:server_event("equip inventory item", self.client, id, args.index, slot)
				end
			else
				Main.messaging:server_event("remove inventory item", self.client, id, args.index)
			end
		end,
		["inventory-equipped"] = function()
			Main.messaging:server_event("equip inventory item", self.client, id, args.index, args.slot)
		end,
		["inventory-unequipped"] = function()
			Main.messaging:server_event("unequip inventory item", self.client, id, args.index)
		end,
		["inventory-subscribed"] = function()
			local owner = Main.objects:find_by_id(id)
			local spec = owner.spec
			if not self.inventory_subscriptions then self.inventory_subscriptions = {} end
			self.inventory_subscriptions[id] = args.inventory
			Main.messaging:server_event("create inventory", self.client, id, spec.type, spec.name, args.inventory.size, (id == self:get_id()))
		end,
		["inventory-unsubscribed"] = function()
			if not self.inventory_subscriptions then return end
			self.inventory_subscriptions[id] = nil
			Main.messaging:server_event("close inventory", self.client, id)
		end
	}
	local fun = funs[args.type]
	if fun then fun(args) end
	Actor.handle_inventory_event(self, args)
end

--- Called when a modifier is removed.
-- @param self Object.
-- @param name Modifier name.
Player.removed_modifier = function(self, name)
	Main.messaging:server_event("remove modifier", self.client, name)
	return Actor.removed_modifier(self, name)
end

Player.respawn = function(self)
	-- Teleport the player.
	local home = self:set_spawn_point()
	self:teleport{position = home}
	-- Resurrect the player.
	self:action("resurrect")
	-- Spawn the player.
	local spec = ModifierSpec:find_by_name("respawn")
	if not spec then return end
	local modifier = Modifier(spec, self, self)
	if modifier:start(1) then
		self:add_modifier(modifier)
	end
end

--- Updates the state of the player.
-- @param self Object.
-- @param secs Seconds since the last update.
Player.update = function(self, secs)
	if not self:get_visible() then return end
	if self:has_server_data() then
		if self.client then
			-- Check for bugged characters just in case.
			if not self:get_visible() or not self.vision then return self:detach() end
			-- Prevent sectors from unloading if a player is present.
			self:refresh(self.vision:get_radius())
		end
		-- Update vision.
		if self.vision then
			self.vision:update(secs)
		end
		-- Update inventory subscriptions.
		self:update_inventory_subscriptions()
		-- Verify crafting device distance.
		if self.crafting_device then
			if not self:can_reach_object(self.crafting_device) then
				self:set_crafting_device()
			end
		end
	end
	-- Update the base class.
	Actor.update(self, secs)
end

--- Closes unreachable inventories.
-- @param self Object.
Player.update_inventory_subscriptions = function(self)
	if not self.inventory_subscriptions then return end
	if not self.inventory:is_subscribed(self) then
		self.inventory:subscribe(self, function(args) self:handle_inventory_event(args) end)
	end
	for id,inv in pairs(self.inventory_subscriptions) do
		local object = Main.objects:find_by_id(id)
		if not object or not self:can_reach_object(object) then
			inv:unsubscribe(self)
		end
	end
end

--- Updates the skills and related attributes of the player.
-- @param self Player.
Player.update_skills = function(self)
	-- Recalculate the skills.
	Actor.update_skills(self)
	-- Send an update to the client.
	Main.messaging:server_event("update skills", self.client, self.skills:get_names())
end

--- Gets the admin privilege of the player.
-- @param self Player.
-- @return True if admin. False otherwise.
Player.get_admin = function(self)
	local account = self.account
	if not account then return end
	return Server.config.admins[account.login]
end

--- Sets the admin privilege of the player.
-- @param self Player.
-- @param value True to make an admin. False otherwise.
Player.set_admin = function(self, value)
	local account = self.account
	if not account then return end
	Server.config.admins[account.login] = value and true or nil
	Server.config:save()
end

--- Sets the client controlling the player.
-- @param self Player.
-- @param client Client ID.
Player.set_client = function(self, client)
	self.client = client
	self.vision = PlayerVision(self)
end

--- Sets the crafting device used by the player.
-- @param self Player.
-- @param object Crafting device, or nil.
-- @param mode Crafting mode, or nil.
Player.set_crafting_device = function(self, object, mode)
	self.crafting_mode = mode
	self.crafting_device = object
	Main.messaging:server_event("craft", self.client, mode or "default")
end

--- Gets the spawn point of the player.
-- @param self Object.
-- @return Spawn point vector in world space, or nil.
Player.get_spawn_point = function(self)
	if not self.account then return end
	return self.account.spawn_point
end

--- Sets the spawn point of the player.
-- @param self Object.
-- @param name Spawn point name.
-- @return Spawn point vector in world space, or nil.
Player.set_spawn_point = function(self, name)
	-- Select the spawn point.
	local home
	if not name or name == "Home" then
		home = self.account and self.account.spawn_point
	else
		local r = Patternspec:find{name = spawnpoint}
		if r and not r.spawn_point then r = nil end
		if r then home = r:get_spawn_point_world() end
	end
	-- Use the default if not found.
	if not home then
		home = Utils:get_player_spawn_point()
	end
	-- Set the spawn point vector.
	if self.account then
		self.account.spawn_point = home
	end
	return home
end

Player.get_storage_type = function(self)
	return "player"
end

return Player
