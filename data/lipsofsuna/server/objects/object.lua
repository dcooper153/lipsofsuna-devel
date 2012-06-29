require "system/object"

ServerObject = Class(Object)
ServerObject.class_name = "ServerObject"

local objspec = Spec{name = "object", type = "object"}

ServerObject.new = function(clss, args)
	local self = Object.new(clss, {id = args and args.id})
	self.inventory = Inventory{id = self.id}
	if args then
		for k,v in pairs(args) do
			self[k] = v
		end
	end
	return self
end

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
-- @return True if the hit was processed.
ServerObject.contact_cb = function(self, result)
	if not self.contact_args then
		self.contact_args = nil
		self.contact_events = false
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
	self.contact_events = false
	self.realized = false
	return true
end

--- Default add enemy call.
-- @param self Object.
-- @param object Object to add to the list of enemies.
ServerObject.add_enemy = function(self, object)
end

--- Plays an animation.
-- @param self Object.
-- @param name Animation name.
-- @param force_temporary Forces the animation to be temporary in the server side.
-- @return True if started a new animation.
ServerObject.animate = function(self, name, force_temporary)
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
			self.animations[anim.channel] = {name, Program.time}
		else
			self.animations[anim.channel] = nil
		end
	end
	-- Emit a vision event.
	Vision:event({type = "object-animated", animation = name, object = self})
	return true
end

--- Returns true if the given object is reachable to this object.
-- @param self Object.
-- @param object Object.
-- @return True if reachable.
ServerObject.can_reach_object = function(self, object)
	-- Check for reachability of inventory items.
	if object.parent then
		local parent = ServerObject:find{id = object.parent}
		if not parent then return end
		if not parent.inventory:is_subscribed(self) then return end
		return self:can_reach_object(parent)
	end
	-- Make sure that the target exists in the world.
	if not object.realized then return end
	-- Check the distance from the aim ray center to the bounding box of the target.
	local center = self.position
	if self.spec.aim_ray_center then
		center = center + self.rotation * self.spec.aim_ray_center
	end
	local bounds = object.bounding_box
	local diff = object.rotation.conjugate * (center - object.position)
	local dist = bounds:get_distance_to_point(diff)
	return dist <= 5
end

--- Clones the object.
-- @param self Object.
-- @return New object.
ServerObject.clone = function(self)
end

--- Causes the object to take damage.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>amount: Amount of damage.</li>
--   <li>point: Damage point.</li>
--   <li>type: Damage type.</li></ul>
ServerObject.damaged = function(self, args)
end

--- Detaches the object from the world or any container.
-- @param self Object.
ServerObject.detach = function(self)
	-- Detach from world.
	self.realized = false
	-- Detach from inventory.
	if self.parent then
		local parent = ServerObject:find{id = self.parent}
		if parent then
			parent.inventory:remove_object(self)
		end
		self.parent = nil
	end
end

--- Hides the object and purges it from the database.
-- @param self Object to kill.
ServerObject.die = function(self)
	self:detach()
	self:purge()
end

ServerObject.effect = function(self, args)
	if args.effect then
		Vision:event{type = "object-effect", object = self, effect = args.effect}
	end
end

--- Finds an open inventory.
-- @param self Object.
-- @param id Inventory ID.
-- @return Inventory or nil.
ServerObject.find_open_inventory = function(self, id)
	local obj = ServerObject:find{id = id}
	if not obj then return end
	if not object.inv:is_subscribed(self) then return end
	return object.inv
end

--- Finds a targeted object.
-- @param self Object.
-- @param where Inventory number or zero for world.
-- @param what Inventory slot number or object number for world.
-- @return Object or nil.
ServerObject.find_target = function(self, where, what)
	if where == 0 then
		return ServerObject:find{id = what, point = self.position, radius = 5}
	else
		local obj = ServerObject:find{id = where}
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
ServerObject.fire = function(self, args)
	if not args.owner or not args.feat then return end
	-- Enable collision callback.
	if args.collision then
		self.contact_args = args
		self.contact_events = true
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
		self.flags = Bitwise:bor(self.flags or 0, Protocol.object_flags.SPEEDLINE)
	end
	-- Store attack charge.
	self.charge = args.charge
	-- Add the projectile to the world.
	local src,dst = args.owner:get_attack_ray()
	self:detach()
	self.owner = args.owner
	self.position = src
	if args.owner.tilt then
		self.rotation = args.owner.rotation * args.owner.tilt
	else
		self.rotation = args.owner.rotation
	end
	self.velocity = (dst - src):normalize() * (args.speed or 20)
	self.realized = true
end

--- Gets a dialog variable by name.
-- @param self Object.
-- @param name Variable name.
-- @return Variable value, or nil.
ServerObject.get_dialog_variable = function(self, name)
	return Serialize:get_dialog_variable(self, name)
end

--- Sets a dialog variable.
-- @param self Object.
-- @param name Variable name.
-- @param value Variable value.
ServerObject.set_dialog_variable = function(self, name, value)
	return Serialize:set_dialog_variable(self, name, value)
end

--- Gets all dialog variable for the object.
-- @param self Object.
-- @return List of variables.
ServerObject.get_dialog_variables = function(self, name)
	return Serialize:get_dialog_variables(self)
end

ServerObject.get_equip_value = function(self, user)
	local score = 50 * self:get_armor_class(user)
	for k,v in pairs(self:get_weapon_influences(user)) do
		if k ~= "hatchet" then
			score = score + v
		end
	end
end

--- Gets a free object ID.
-- @param clss Object class.
-- @return Free object ID.
ServerObject.get_free_id = function(clss)
	while true do
		local id = math.random(0x0000001, 0x0FFFFFF)
		if not ServerObject:find{id = id} then
			local rows = Serialize.db:query([[SELECT id FROM object_data WHERE id=?;]], {id})
			if not rows[1] then
				return id
			end
		end
	end
end

--- Gets the spell effects known by the object.
-- @param self Object.
-- @return List of strings.
ServerObject.get_known_spell_effects = function(self)
	return {}
end

--- Gets the spell types known by the object.
-- @param self Object.
-- @return Dictionary of booleans.
ServerObject.get_known_spell_types = function(self)
	return {}
end

--- Gets a full name string for the object.
-- @param self Object.
-- @return String.
ServerObject.get_name_with_count = function(self)
	local name = self.name or "unnamed object"
	local count = self.count
	if count > 1 then
		name = "" .. count .. "x " .. name
	end
	return name
end

--- Returns the range of tiles occupied by the object.
-- @param self Object.
-- @return Start vector, size vector.
ServerObject.get_tile_range = function(self)
	-- TODO: Should depend on actor spec.
	local size = Vector(1,self.spec.type == "actor" and 2 or 1,1)
	local src = self.position * Voxel.tile_scale
	src.x = math.floor(src.x)
	src.y = math.floor(src.y + 0.3)
	src.z = math.floor(src.z)
	return src, src + size - Vector(1,1,1)
end

--- Inflicts a modifier on the object.
-- @param self Object.
-- @param name Modifier name.
-- @param strength Modifier strength.
-- @param args Args passed to modifier
ServerObject.inflict_modifier = function(self, name, strength, args)
end

--- Loots the object.
-- @param self Object.
-- @param user Object doing the looting.
ServerObject.loot = function(self, user)
	if self.inventory.size > 0 then
		self.inventory:subscribe(user, function(args) user:inventory_cb(args) end)
		self:animate("loot")
		self.looted = true
	end
end

--- Merges the objects if they're similar.
-- @param self Object.
-- @param object Object to merge to this one.
-- @return True if merged successfully.
ServerObject.merge = function(self, object)
	if self.spec == object.spec and self.spec.stacking then
		self.count = self.count + object.count
		object:detach()
		return true
	end
end

ServerObject.purge = function(self)
	Serialize.db:query([[DELETE FROM object_data WHERE id=?;]], {self.id})
	Serialize.db:query([[DELETE FROM object_inventory WHERE id=?;]], {self.id})
	Serialize.db:query([[DELETE FROM object_sectors WHERE id=?;]], {self.id})
	Serialize.db:query([[DELETE FROM object_skills WHERE id=?;]], {self.id})
	Serialize.db:query([[DELETE FROM object_stats WHERE id=?;]], {self.id})
end

--- Sends a chat message to all players near the object.
-- @param self Speaking object.
-- @param msg Message to send.
ServerObject.say = function(self, msg)
	if msg then
		Vision:event{type = "object-speech", object = self, message = msg}
	end
end

--- Sends a packet to the client controlling the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>packet: Packet.</li>
--   <li>reliable: False for unreliable.</li></ul>
ServerObject.send = function(self, args)
	if self.client then
		if type(args) == "string" then
			local packet = Packet(packets.MESSAGE, "string", args)
			Network:send{client = self.client, packet = packet, reliable = true}
		elseif args.class_name == "Packet" then
			Network:send{client = self.client, packet = args, reliable = true}
		else
			Network:send{client = self.client, packet = args.packet, reliable = args.reliable}
		end
	end
end

--- Fixes the position of a stuck object.
-- @param self Object.
-- @return True if fixing succeeded.
ServerObject.stuck_fix = function(self)
	do return true end
	-- Get the tile position of the object.
	local src = self:get_tile_range()
	-- Find the closest empty tile.
	-- FIXME: The object doesn't necessarily fit inside one tile.
	local t,p = Voxel:find_tile{match = "empty", point = src * Voxel.tile_size, radius = 5 * Voxel.tile_size}
	if not t then return end
	-- Move the object to the empty tile.
	self.position = (p + Vector(0.5, 0.2, 0.5)) * Voxel.tile_size
	return true
end

--- Subtracts stacked objects.
-- @param self Object.
-- @param count: Count to subtract.
ServerObject.subtract = function(self, count)
	local c = count or 1
	if self.count > c then
		self.count = self.count - c
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
ServerObject.teleport = function(self, args)
	-- Set the position.
	if args.marker then
		local marker = Marker:find{name = args.marker}
		if not marker or not marker.position then return end
		self.position = marker.position + Vector(0, 2, -1)
	elseif args.region then
		local reg = Patternspec:find{name = args.region}
		if not reg then return end
		self.position = reg.spawn_point_world
	elseif args.position then
		self.position = args.position
	else return end
	self.velocity = Vector()
	self.realized = true
	return true
end

--- Updates the environment of the object and tries to fix it if necessary.
-- @param self Object.
-- @param secs Seconds since the last update.
-- @return Boolean and environment statistics. The boolean is true if the object isn't permanently stuck.
ServerObject.update_environment = function(self, secs)
	if not self.env_timer then self.env_timer = 0 end
	self.env_timer = self.env_timer + secs
	if self.env_timer < 2 then return true end
	-- Count tiles affecting us.
	if not self.realized then return true end
	local src,dst = self:get_tile_range()
	local res = Voxel:check_range(src, dst)
	-- Stuck handling.
--[[	if res.solid > 0 then
		self.stuck = (self.stuck or 0) + 2
		if self.stuck < 10 then
			self:stuck_fix()
		else
			print("Warning: " .. (self.spec.name or "an object") .. " was deleted because it was permanently stuck!")
			self.realized = false
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
ServerObject.read_db = function(self, db)
end

--- Writes the object to a database.
-- @param self Object.
-- @param db Database.
ServerObject.write_db = function(self, db)
end

ServerObject:add_getters{
	admin = function(s)
		local a = rawget(s, "account")
		return a and Config.inst.admins[a.login]
	end,
	count = function(s)
		return rawget(s, "__count") or 1
	end,
	spec = function(s)
		return rawget(s, "__spec") or objspec
	end}

ServerObject:add_setters{
	admin = function(s, v)
		Config.inst.admins[s] = v and true or nil
		Config.inst:save()
	end,
	count = function(s, v)
		-- Store the new count.
		if s.count == v then return end
		rawset(s, "__count", v ~= 0 and v or nil)
		-- Update the inventory containing the object.
		if s.parent then
			local parent = ServerObject:find{id = s.parent}
			if parent then
				parent.inventory:update_object(s)
			else
				s.parent = nil
			end
		end
	end,
	spec = function(s, v)
		rawset(s, "__spec", v)
	end}
