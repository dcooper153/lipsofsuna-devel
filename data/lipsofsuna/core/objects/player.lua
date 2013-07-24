--- TODO:doc
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
local Marker = require("core/marker")
local Modifier = require("core/server/modifier")
local ModifierSpec = require("core/specs/modifier")
local Packet = require("system/packet")
local Sector = require("system/sector")
local Vision = require("system/vision")

--- TODO:doc
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
	self.vision_timer = 0
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

Player.set_client = function(self, client)
	self.client = client
	self.vision = Vision(self:get_id(), self.manager)
	self.vision:set_cone_factor(0.5)
	self.vision:set_cone_angle(math.pi/2.5)
	self.vision:set_enabled(true)
	self.vision.object = self
	self.vision.callback = function(args) self:vision_cb(args) end
	self.vision.terrain = {}
	self:update_vision_radius()
	self.vision:update()
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

--- Sets the crafting device used by the player.
-- @param self Object.
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
			self.vision_timer = self.vision_timer + secs
			if self.vision_timer > 0.1 then
				self.vision_timer = 0
				self:update_vision_radius()
				self.vision:update()
				self:update_map()
				self:update_inventory_subscriptions()
			end
		end
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

Player.update_map = function(self)
	-- Discover map markers.
	for k,v in pairs(Marker.dict_discoverable) do
		if (self:get_position() - v.position).length < 3 * self.vision:get_radius() then
			v:unlock()
		end
	end
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
-- @param self Object.
Player.update_skills = function(self)
	-- Recalculate the skills.
	Actor.update_skills(self)
	-- Send an update to the client.
	Main.messaging:server_event("update skills", self.client, self.skills:get_names())
end

--- Updates the vision radius of the player.<br/>
-- The vision system needs the direction and position of the player, so we
-- update it here.
-- @param self Object.
Player.update_vision_radius = function(self)
	self.vision:set_direction(Vector(0,0,-1):transform(self:get_rotation()))
	self.vision:set_position(self:get_position())
	self.vision:set_radius(50)
end

Player.vision_cb = function(self, args)
	local funs
	funs =
	{
		["object-animated"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object animated", self.client, o:get_id(), args.animation or "", args.time or 0.0, args.variant)
		end,
		["object attack"] = function(args)
			local o = args.object
			Main.messaging:server_event("object attack", self.client, o:get_id(), args.move, args.variant)
		end,
		["object-beheaded"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object beheaded", self.client, o:get_id(), o:get_beheaded())
		end,
		["object-combat"] = function(args)
			local o = args.object
			Main.messaging:server_event("object combat", self.client, o:get_id(), o:get_combat_hint())
		end,
		["object-dead"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object dead", self.client, o:get_id(), args.dead)
		end,
		["object-dialog"] = function(args)
			local o = args.object
			local mine = (o.dialog and o.dialog.user == self or false)
			if args.choices then
				Main.messaging:server_event("object dialog choice", self.client, o:get_id(), mine, args.choices)
			elseif args.message then
				Main.messaging:server_event("object dialog say", self.client, o:get_id(), mine, args.character or "", args.message)
			else
				Main.messaging:server_event("object dialog none", self.client, o:get_id())
			end
		end,
		["object-effect"] = function(args)
			local o = args.object
			Main.messaging:server_event("object effect", self.client, o:get_id(), args.effect)
		end,
		["object-hidden"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object hidden", self.client, o:get_id())
		end,
		["object-motion"] = function(args)
			local o = args.object
			if o:get_static() then return end
			Main.messaging:server_event("object moved", self.client, o:get_id(), o:get_position(), o:get_rotation(), o:get_tilt_angle(), o:get_velocity())
		end,
		["object-shown"] = function(args)
			-- Don't send static objects.
			local o = args.object
			if o:get_static() then return end
			-- Notify the client.
			Main.messaging:server_event("object shown", self.client, o == self, o)
			-- Wake up the AI.
			if o.ai then o.ai:refresh() end
			local flags = o.flags or 0
			-- Update map vision.
			if o == self then self:update_map() end
		end,
		["object-speech"] = function(args)
			local o = args.object
			Main.messaging:server_event("object speech", self.client, o:get_id(), args.message)
		end,
		["stat changed"] = function(args)
			local o = args.object
			local s = o.stats
			if not s then return end
			local v = s:get_skill(args.name)
			if not v then return end
			if v.prot == "public" or self == o then
				Main.messaging:server_event("object stat", self.client, o:get_id(), args.name, args.value, args.maximum, math.ceil(args.value - args.value_prev + 0.5))
			end
		end,
		["object-equip"] = function(args)
			-- If the player is subscribed to the inventory of the object, the
			-- equip message is already sent by the inventory listener.
			local id = args.object:get_id()
			if args.object.inventory:is_subscribed(self) then return end
			-- The contents of the inventory slot must be revealed to the client
			-- since it would otherwise have no information on the equipped item.
			Main.messaging:server_event("add inventory item", self.client, id, args.index, args.item.spec.name, args.item:get_count())
			-- Send the equip message.
			Main.messaging:server_event("equip inventory item", self.client, id, args.index, args.slot)
		end,
		["object-unequip"] = function(args)
			-- If the player is subscribed to the inventory of the object, the
			-- unequip message is already sent by the inventory listener.
			local id = args.object:get_id()
			if args.object.inventory:is_subscribed(self) then return end
			-- Send the unequip message.
			Main.messaging:server_event("unequip inventory item", self.client, id, args.index)
			-- The client doesn't need the item information of the unsubscribed
			-- inventory anymore so we can clear the item.
			Main.messaging:server_event("remove inventory item", self.client, id, args.index)
		end,
		["voxel-block-changed"] = function(args)
			if self.client == -1 then return end
			local id = Main.messaging:get_event_id("update terrain")
			local x,y,z = Sector:get_block_offset_by_block_id(args.index)
			local packet = Packet(id, "uint32", x, "uint32", y, "uint32", z)
			-- FIXME
			--Voxel:get_block(x, y, z, packet)
			--Main.messaging:server_event("update terrain", self.client, packet)
		end,
		["world-effect"] = function(args)
			Main.messaging:server_event("world effect", self.client, args.point, args.effect)
		end
	}
	local fun = funs[args.type]
	if fun then fun(args) end
end

--- Writes the object to a database.
-- @param self Object.
-- @param db Database.
Player.write_db = function(self, db)
	-- Write the object data.
	Actor.write_db(self, db)
	-- Write account information.
	if self.client and self.account then
		Server.account_database:save_account(self.account, self)
	end
end

Player.get_admin = function(self)
	local account = rawget(self, "account")
	if not account then return end
	return Server.config.admins[account.login]
end

Player.set_admin = function(self, v)
	local account = rawget(self, "account")
	if not account then return end
	Server.config.admins[account.login] = v and true or nil
	Server.config:save()
end

Player.get_storage_sector = function(self)
	if self.client then return end
	return self:get_sector()
end

Player.get_storage_type = function(self)
	return "player"
end

return Player
