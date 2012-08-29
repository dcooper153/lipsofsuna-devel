--- The base class for game objects.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.simulation
-- @alias SimulationObject

local Class = require("system/class")
local Inventory = require("core/server/inventory")
local Object = require("system/object")
local Model = require("system/model")
local Settings = require("common/settings")
local Timer = require("system/timer")
local ClientRenderObject = not Settings.server and require("core/client/client-render-object")
local MovementPrediction = not Settings.server and require("core/client/movement-prediction")

--- The base class for game objects.
-- @type SimulationObject
local SimulationObject = Class("SimulationObject", Object)

--- Creates a new simulation object.
-- @param clss SimulationObject class.
-- @param id Unique ID, or nil for automatically selected.
-- @return SimulationObject.
SimulationObject.new = function(clss, id)
	local self = Object.new(clss)
	-- Select a unique ID.
	if id then
		local old = Game.objects.objects_by_id[id]
		if old then old:detach() end
		self:set_id(id)
	else
		self:set_id(Game.objects:get_free_id())
	end
	Game.objects:add(self)
	-- Initialize client and server data.
	self.inventory = Inventory(self:get_id())
	if Game.enable_graphics then
		self.render = ClientRenderObject()
	end
	if Game.enable_prediction then
		self.prediction = MovementPrediction()
	end
	return self
end

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
-- @return True if the hit was processed.
SimulationObject.contact_cb = function(self, result)
	if not self.contact_args then
		self.contact_args = nil
		self:set_contact_events(false)
		return
	end
	if result.object == self.contact_args.owner then return end
	local args = {
		charge = self.contact_args.charge,
		object = result.object,
		owner = self.contact_args.owner,
		point = result.point,
		projectile = self,
		tile = result.tile,
		weapon = self.contact_args.weapon}
	self.contact_args.feat:apply_impulse(args)
	self.contact_args.feat:apply(args)
	self.contact_args = nil
	self:set_contact_events(false)
	self:detach()
	return true
end

--- Default add enemy call.
-- @param self Object.
-- @param object Object to add to the list of enemies.
SimulationObject.add_enemy = function(self, object)
end

--- Plays an animation.
-- @param self Object.
-- @param name Animation name.
-- @param force_temporary Forces the animation to be temporary in the server side.
-- @return True if started a new animation.
SimulationObject.animate = function(self, name, force_temporary)
	-- Prevent animation when dead.
	-- This is a simple way to ensure that dead actors look like dead.
	if self.dead then return end
	-- Get the animation spec.
	if not self.spec.get_animation then return end
	local anim = self.spec:get_animation(name)
	if not anim then return end
	-- Maintain channels.
	-- When objects enter the vision of a player, the player class enumerates
	-- through the persistent animations and sends them to the client. We need
	-- to store them so that newly seen objects don't appear unanimated.
	if anim.channel then
		if anim.permanent and not force_temporary then
			if not self.animations then self.animations = {} end
			local prev = self.animations[anim.channel]
			if prev and prev[1] == name then return end
			self.animations[anim.channel] = {name, Program:get_time()}
		else
			self.animations[anim.channel] = nil
		end
	end
	-- Emit a vision event.
	Server:object_event(self, "object-animated", {animation = name, variant = math.random(0, 255)})
	return true
end

--- Returns true if the given object is reachable to this object.
-- @param self Object.
-- @param object Object.
-- @return True if reachable.
SimulationObject.can_reach_object = function(self, object)
	-- Check for reachability of inventory items.
	if object.parent then
		local parent = Game.objects:find_by_id(object.parent)
		if not parent then return end
		if not parent.inventory:is_subscribed(self) then return end
		return self:can_reach_object(parent)
	end
	-- Make sure that the target exists in the world.
	if not object:get_visible() then return end
	-- Check the distance from the aim ray center to the bounding box of the target.
	local center = self:transform_local_to_global(self.spec.aim_ray_center)
	local bounds = object:get_bounding_box()
	local diff = center:subtract(object:get_position()):transform(object:get_rotation().conjugate)
	local dist = bounds:get_distance_to_point(diff)
	return dist <= 5
end

--- Clones the object.
-- @param self Object.
-- @return New object.
SimulationObject.clone = function(self)
end

--- Causes the object to take damage.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>amount: Amount of damage.</li>
--   <li>point: Damage point.</li>
--   <li>type: Damage type.</li></ul>
SimulationObject.damaged = function(self, args)
end

--- Detaches the object from the world or any container.
-- @param self Object.
SimulationObject.detach = function(self)
	-- Detach from the world.
	self:set_visible(false)
	-- Detach from inventories.
	if self:has_server_data() then
		if self.parent then
			local parent = Game.objects:find_by_id(self.parent)
			if parent then
				parent.inventory:remove_object(self)
			end
			self.parent = nil
		end
	end
	-- Detach client data.
	if self:has_client_data() then
		self.render:clear()
	end
end

--- Hides the object and purges it from the database.
-- @param self Object.
SimulationObject.die = function(self)
	self:detach()
	self:purge()
end

--- Plays an effect for the object.
-- @param self Object.
-- @param args Effect arguments.
SimulationObject.effect = function(self, args)
	if not args.effect then return end
	Server:object_event(self, "object-effect", {effect = args.effect})
end

--- Finds an open inventory.
-- @param self Object.
-- @param id Inventory ID.
-- @return Inventory or nil.
SimulationObject.find_open_inventory = function(self, id)
	local obj = Game.objects:find_by_id(id)
	if not obj then return end
	if not object.inv:is_subscribed(self) then return end
	return object.inv
end

--- Finds a targeted object.
-- @param self Object.
-- @param where Inventory number or zero for world.
-- @param what Inventory slot number or object number for world.
-- @return Object or nil.
SimulationObject.find_target = function(self, where, what)
	if where == 0 then
		return Game.objects:find_by_id_and_point(what, self:get_position(), 5)
	else
		local obj = Game.objects:find_by_id(id)
		if not obj then return end
		if obj.inventory:is_subscribed(self) then
			return obj.inventory:get_object_by_index(what)
		end
	end
	return nil
end

--- Fires or throws the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>charge: Charge time of the attack.</li>
--   <li>collision: Trigger at collision.</li>
--   <li>feat: Feat.</li>
--   <li>owner: Object firing the projectile.</li>
--   <li>speed: Initial speed.</li>
--   <li>speedline: True if the object should have a speed line effect.</li>
--   <li>timer: Trigger at timeout.</li>
--   <li>weapon: Used weapon.</ul>
-- @return True on success.
SimulationObject.fire = function(self, args)
	if not args.owner or not args.feat then return end
	-- Enable collision callback.
	if args.collision then
		self.contact_args = args
		self:set_contact_events(true)
	end
	-- Enable destruction timer.
	if args.timer then
		Timer{delay = args.timer, func = function(timer)
			self:die()
			timer:disable()
		end}
	end
	-- Initialize speed lines.
	if args.speedline then
		self.speedline = true
	end
	-- Store attack charge.
	self.charge = args.charge
	-- Add the projectile to the world.
	local src,dst = args.owner:get_attack_ray()
	self:detach()
	self.owner = args.owner
	self:set_position(src)
	if args.owner.tilt then
		self:set_rotation(args.owner:get_rotation() * args.owner.tilt)
	else
		self:set_rotation(args.owner:get_rotation())
	end
	self:set_velocity(dst:copy():subtract(src):normalize():multiply(args.speed or 20))
	self:set_visible(true)
end

--- Returns true if the object is used in client side.
-- @param self Object.
-- @return True if used, false if not.
SimulationObject.has_client_data = function(self)
	return self.render ~= nil
end

--- Returns true if the object is used in server side.
-- @param self Object.
-- @return True if used, false if not.
SimulationObject.has_server_data = function(self)
	return Server.initialized
end

--- Gets the animation profile for the object.
-- @param self Object.
-- @return String.
SimulationObject.get_animation_profile = function(self)
	if self.animation_profile then return self.animation_profile end
	if self.spec.preset then
		local preset = Actorpresetspec:find{name = self.spec.preset}
		if preset.animation_profile then return preset.animation_profile end
	end
	return "default"
end

--- Gets a dialog variable by name.
-- @param self Object.
-- @param name Variable name.
-- @return Variable value, or nil.
SimulationObject.get_dialog_variable = function(self, name)
	return Server.quest_database:get_dialog_variable(self, name)
end

--- Sets a dialog variable.
-- @param self Object.
-- @param name Variable name.
-- @param value Variable value.
SimulationObject.set_dialog_variable = function(self, name, value)
	return Server.quest_database:set_dialog_variable(self, name, value)
end

--- Gets all dialog variable for the object.
-- @param self Object.
-- @return List of variables.
SimulationObject.get_dialog_variables = function(self)
	return Server.quest_database:get_dialog_variables(self)
end

SimulationObject.get_equip_value = function(self, user)
	local score = 50 * self:get_armor_class(user)
	for k,v in pairs(self:get_weapon_influences(user)) do
		if k ~= "hatchet" then
			score = score + v
		end
	end
end

--- Gets the spell effects known by the object.
-- @param self Object.
-- @return List of strings.
SimulationObject.get_known_spell_effects = function(self)
	return {}
end

--- Gets the spell types known by the object.
-- @param self Object.
-- @return Dictionary of booleans.
SimulationObject.get_known_spell_types = function(self)
	return {}
end

--- Gets a full name string for the object.
-- @param self Object.
-- @return String.
SimulationObject.get_name_with_count = function(self)
	local name = self.name or "unnamed object"
	local count = self:get_count()
	if count > 1 then
		name = "" .. count .. "x " .. name
	end
	return name
end

--- Returns the range of tiles occupied by the object.
-- @param self Object.
-- @return Start vector, size vector.
SimulationObject.get_tile_range = function(self)
	-- TODO: Should depend on actor spec.
	local size = Vector(1,self.spec.type == "actor" and 2 or 1,1)
	local src = self:get_position() * Voxel.tile_scale
	src.x = math.floor(src.x)
	src.y = math.floor(src.y + 0.3)
	src.z = math.floor(src.z)
	return src, src + size - Vector(1,1,1)
end

--- Gets the tilt angle of the object.
-- @param self Object.
-- @return Angle in radians.
SimulationObject.get_tilt_angle = function(self)
	if not self.tilt then return 0 end
	return self.tilt.euler[3]
end

--- Sets the tilt angle of the object.
-- @param self Object.
-- @param value Angle in radians.
-- @param predict True to use prediction.
SimulationObject.set_tilt_angle = function(self, value, predict)
	if self.prediction and predict then
		self.prediction:set_target_tilt(value)
	else
		self.tilt = Quaternion{euler = {0, 0, value}}
	end
end

--- Gets the turning angle of the object.
-- @param self Object.
-- @return Angle in radians.
SimulationObject.get_turn_angle = function(self)
	return self:get_rotation().euler[1]
end

--- Inflicts a modifier on the object.
-- @param self Object.
-- @param name Modifier name.
-- @param strength Modifier strength.
-- @param args Args passed to modifier
SimulationObject.inflict_modifier = function(self, name, strength, args)
end

--- Loots the object.
-- @param self Object.
-- @param user Object doing the looting.
SimulationObject.loot = function(self, user)
	if self.inventory.size > 0 then
		self.inventory:subscribe(user, function(args) user:handle_inventory_event(args) end)
		self:animate("loot")
		self.looted = true
		Game.messaging:server_event("inventory show", user.client, self:get_id())
	end
end

--- Merges the objects if they're similar.
-- @param self Object.
-- @param object Object to merge to this one.
-- @return True if merged successfully.
SimulationObject.merge = function(self, object)
	if self.spec == object.spec and self.spec.stacking then
		self:set_count(self:get_count() + object:get_count())
		object:detach()
		return true
	end
end

SimulationObject.purge = function(self)
	Server.object_database:delete_object(self)
end

--- Sends a chat message to all players near the object.
-- @param self Speaking object.
-- @param msg Message to send.
SimulationObject.say = function(self, msg)
	if not msg then return end
	Server:object_event(self, "object-speech", {message = msg})
end

--- Sends a message to the client controlling the object.
-- @param self Object.
-- @param text Message text.
SimulationObject.send_message = function(self, text)
	if not self.client then return end
	Game.messaging:server_event("message", self.client, text)
end

--- Fixes the position of a stuck object.
-- @param self Object.
-- @return True if fixing succeeded.
SimulationObject.stuck_fix = function(self)
	do return true end
	-- Get the tile position of the object.
	local src = self:get_tile_range()
	-- Find the closest empty tile.
	-- FIXME: The object doesn't necessarily fit inside one tile.
	local t,p = Voxel:find_tile{match = "empty", point = src * Voxel.tile_size, radius = 5 * Voxel.tile_size}
	if not t then return end
	-- Move the object to the empty tile.
	self:set_position((p + Vector(0.5, 0.2, 0.5)) * Voxel.tile_size)
	return true
end

--- Subtracts stacked objects.
-- @param self Object.
-- @param count Count to subtract.
SimulationObject.subtract = function(self, count)
	local c = count or 1
	local old = self:get_count()
	if old > c then
		self:set_count(old - c)
	else
		self:detach()
		self:purge()
	end
end

--- Teleports the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>marker: Map marker name.</li>
--   <li>position: World position.</li>
--   <li>region: Region name.</li></ul>
-- @return True on success.
SimulationObject.teleport = function(self, args)
	-- Set the position.
	if args.marker then
		local marker = Marker:find{name = args.marker}
		if not marker or not marker.position then return end
		self:set_position(marker.position + Vector(0, 2, -1))
	elseif args.region then
		local reg = Patternspec:find{name = args.region}
		if not reg then return end
		self:set_position(reg:get_spawn_point_world())
	elseif args.position then
		self:set_position(args.position)
	else return end
	self:set_velocity(Vector())
	self:set_visible(true)
	return true
end

--- Updates the object.
-- @param self Object.
-- @param secs Seconds since the last update.
SimulationObject.update = function(self, secs)
	if not self:get_visible() then return end
	if self:has_client_data() then
		-- Update sound.
		if self.animated then
			self:update_sound(secs)
		end
		-- Interpolate the position.
		if self.prediction and self.prediction.enabled then
			self.prediction:update(secs)
			self:set_position(self.prediction:get_predicted_position())
			if self.dead or self ~= Client.player_object then
				self:set_rotation(self.prediction:get_predicted_rotation())
				self:set_tilt_angle(self.prediction:get_predicted_tilt())
			end
		end
		-- Update the render object.
		if self:has_server_data() then
			self.render:set_position(self:get_position())
			self.render:set_rotation(self:get_rotation())
		end
	end
end

--- Updates the environment of the object and tries to fix it if necessary.
-- @param self Object.
-- @param secs Seconds since the last update.
-- @return Boolean and environment statistics. The boolean is true if the object isn't permanently stuck.
SimulationObject.update_environment = function(self, secs)
	if not self.env_timer then self.env_timer = 0 end
	self.env_timer = self.env_timer + secs
	if self.env_timer < 2 then return true end
	-- Count tiles affecting us.
	if not self:get_visible() then return true end
	local src,dst = self:get_tile_range()
	local res = Voxel:check_range(src, dst)
	-- Stuck handling.
--[[	if res.solid > 0 then
		self.stuck = (self.stuck or 0) + 2
		if self.stuck < 10 then
			self:stuck_fix()
		else
			print("Warning: " .. (self.spec.name or "an object") .. " was deleted because it was permanently stuck!")
			self:detach()
			return nil, res
		end
	elseif self.stuck then
		if self.stuck > 1 then
			self.stuck = self.stuck -1
		else
			self.stuck = nil
		end
	end--]]
	return true, res
end

--- Reads the object from a database.
-- @param self Object.
-- @param db Database.
SimulationObject.read_db = function(self, db)
	Server.object_database:load_fields(self)
end

--- Writes the object to a database.
-- @param self Object.
-- @param db Database.
SimulationObject.write_db = function(self, db)
	-- Write the object data.
	local id = self:get_id()
	db:query([[REPLACE INTO object_data (id,type,spec,dead) VALUES (?,?,?,?);]],
		{id, self:get_storage_type(), self.spec.name, self.dead and 1 or 0})
	-- Write the serializer fields.
	db:query([[DELETE FROM object_fields WHERE id=?;]], {id})
	self.serializer:write(self, function(name, value)
		db:query([[REPLACE INTO object_fields (id,name,value) VALUES (?,?,?);]], {id, name, value})
	end)
	-- Write the sector information.
	local sector = self:get_storage_sector()
	if sector then
		if self.spec.important then
			db:query([[REPLACE INTO object_sectors (id,sector,time) VALUES (?,?,?);]], {id, sector, nil})
		else
			db:query([[REPLACE INTO object_sectors (id,sector,time) VALUES (?,?,?);]], {id, sector, 0})
		end
	else
		db:query([[DELETE FROM object_sectors where id=?;]], {id})
	end
end

--- Sets the dialog state of the object.
-- @param self Object.
-- @param type Dialog type. ("choice"/"message"/nil)
-- @param args List of choices, the message arguments, or nil.
SimulationObject.set_dialog = function(self, type, args)
	assert(self:has_client_data())
	-- Update the dialog state.
	if type == "choice" then
		self.dialog = {type = type, choices = args}
	elseif type == "message" then
		self.dialog = {type = type, character = args.character, message = args.message}
	else
		self.dialog = nil
	end
	-- Update the dialog UI.
	if Client.active_dialog_object == self then
		Ui:restart_state()
	end
end

--- Finds a node from the graphical skeleton of the object.<br/>
--
-- This only works when running in the graphical mode and the skeleton
-- has been loaded. If these are not the case, nil is returned.
--
-- @param self Object.
-- @param ... Arguments.
SimulationObject.find_node = function(self, ...)
	if not self.render then return end
	return self.render:find_node(...)
end

--- Plays footstep sounds for actors.
-- @param self Object.
-- @param secs Seconds since the last update.
SimulationObject.update_sound = function(self, secs)
	assert(self:has_client_data())
end

--- Gets the stack count of the object.
-- @param self Object.
-- @return Count.
SimulationObject.get_count = function(self)
	return 1
end

--- Gets the bounding box of the object.
-- @param self Object.
-- @return Bounding box.
SimulationObject.get_bounding_box = function(self)
	if self.render then
		local b = self.render:get_bounding_box()
		if b then return b end
	end
	return Object.get_bounding_box(self)
end

--- Gets the model name of the object.
-- @param self Object.
-- @return Model name, or empty string.
SimulationObject.get_model_name = function(self)
	return self.model_name or ""
end

--- Sets the model of the object by name.
-- @param self Object.
-- @param v Model name.
SimulationObject.set_model_name = function(self, v)
	self.model_name = v
	if self:get_visible() then
		local model = v and Main.models:find_by_name(v)
		self:set_model(model)
	end
end

--- Sets the position of the object.
-- @param self Object.
-- @param value Vector.
-- @param predict True to set the predicted value, false for the actual value.
SimulationObject.set_position = function(self, value, predict)
	if self.prediction and predict then
		self.prediction:set_target_position(value)
	else
		Object.set_position(self, value)
		if self.render then
			self.render:set_position(value)
		end
	end
end

--- Sets the rotation of the object.
-- @param self Object.
-- @param value Vector.
-- @param predict True to set the predicted value, false for the actual value.
SimulationObject.set_rotation = function(self, value, predict)
	if self.prediction and predict then
		self.prediction:set_target_rotation(value)
	else
		Object.set_rotation(self, value)
		if self.render then
			self.render:set_rotation(value)
		end
	end
end

--- Sets the visibility of the object.<br/>
-- @param self Object.
-- @param v True for visible, false for hidden.
SimulationObject.set_visible = function(self, v)
	-- Call the base class.
	Object.set_visible(self, v)
	-- Set the activation status.
	Game.objects:activate_object(self, v)
	-- Ensure that visible objects have their models loaded.
	if v and not self:get_model() then
		local name = self:get_model_name()
		if name then self:set_model(Main.models:find_by_name(name)) end
	end
end

--- Sets the velocity of the object.
-- @param self Object.
-- @param value Velocity vector.
-- @param predict True to use prediction
SimulationObject.set_velocity = function(self, value, predict)
	if self.prediction and predict then
		self.prediction:set_target_velocity(value)
	else
		Object.set_velocity(self, value)
	end
end

--- Gets the spec of the object.
-- @param self Object.
-- @return Spec.
SimulationObject.get_spec = function(self)
	return rawget(self, "__spec")
end

--- Sets the spec of the object.
-- @param self Object.
-- @param value Spec.
SimulationObject.set_spec = function(self, value)
	rawset(self, "__spec", value)
	self.spec = value
end

return SimulationObject