require "server/objects/actor"

Player = Class(Actor)
Player.class_name = "Player"
Player.clients = {}

Player.find = function(clss, args)
	if args and args.client then
		return clss.clients[args.client]
	end
	return Actor.find(clss, args)
end

--- Creates a new player object.
-- @param clss Player class.
-- @param args Arguments.<ul>
--   <li>account: Account data.</li>
--   <li>angular: Angular velocity.</li>
--   <li>body_scale: Scale factor of the body.</li>
--   <li>body_style: Body style defined by an array of scalars.</li>
--   <li>eye_style: Eye style defined by an array of {style, red, green, blue}.</li>
--   <li>hair_style: Hair style defined by an array of {style, red, green, blue}.</li>
--   <li>id: Unique object ID or nil for a random free one.</li>
--   <li>jumped: Jump timer.</li>
--   <li>name: Name of the actor.</li>
--   <li>physics: Physics mode.</li>
--   <li>position: Position vector of the actor.</li>
--   <li>rotation: Rotation quaternion of the actor.</li>
--   <li>skin_style: Skin style defined by an array of {style, red, green, blue}.</li>
--   <li>spec: Actorspec of the actor.</li>
--   <li>realized: True to add the object to the simulation.</li></ul>
-- @return Player.
Player.new = function(clss, args)
	local self = Actor.new(clss, args)
	self.account = args.account
	self.inventory_subscriptions = {}
	self.running = true
	self.vision_timer = 0
	self:calculate_speed()
	return self
end

Player.attack_charge_start = function(self)
	if self.cooldown then return end
	local weapon = self:get_weapon()
	if weapon and weapon.spec.animation_charge then
		self:animate(weapon.spec.animation_charge, true)
	else
		self:animate("charge punch", true)
	end
	self.attack_charge = Program.time
end

Player.attack_charge_end = function(self, args)
	-- Get the feat type.
	local anim = nil
	local weapon = self:get_weapon()
	if not weapon or weapon.spec.categories["melee"] then
		anim = "right hand"
	elseif weapon.spec.categories["ranged"] then
		anim = "ranged"
	elseif weapon.spec.categories["throwable"] then
		anim = "throw"
	elseif weapon.spec.categories["build"] then
		anim = "build"
	end
	-- Initialize the feat.
	local feat = Feat{animation = anim}
	feat:add_best_effects{user = self}
	-- Perform the feat.
	if not feat:perform{stop = false, user = self} then
		self:animate("charge cancel")
	end
	self.attack_charge = nil
end

Player.set_client = function(self, client)
	self.client = client
	self.vision = Vision{cone_factor = 0.5, cone_angle = math.pi/2.5, enabled = true, object = self, radius = 10, callback = function(args) self:vision_cb(args) end}
	self.vision.terrain = {}
	self:update_vision_radius()
	self.vision:update()
	self.inventory:subscribe(self, function(args) self:inventory_cb(args) end)
end

--- Causes the player to die and respawn.
-- @param self Player.
Player.die = function(self)
	if Actor.die(self) then
		self:send{packet = Packet(packets.MESSAGE, "string", "You have died...")}
	else
		self:send{packet = Packet(packets.MESSAGE, "string", "Saved by Sanctuary.")}
	end
end

Player.disable = function(self, keep)
	if self.vision then
		self.vision.enabled = false
	end
	if not keep and self.client then
		self:send{packet = Packet(packets.CHARACTER_CREATE)}
		Player.clients[self.client] = nil
		self.client = nil
	end
end

Player.detach = function(self, keep)
	self:disable()
	self.realized = false
end

--- Gets the spell effects known by the object.
-- @param self Object.
-- @return Dictionary of booleans.
Player.get_known_spell_effects = function(self)
	return Unlocks.unlocks["spell effect"] or {}
end

--- Gets the spell types known by the object.
-- @param self Object.
-- @return Dictionary of booleans.
Player.get_known_spell_types = function(self)
	local ret = {}
	local base = Actor.get_known_spell_types(self)
	local unlock = Unlocks.unlocks["spell type"]
	for k in pairs(base) do ret[k] = true end
	if unlock then
		for k in pairs(unlock) do ret[k] = true end
	end
	return ret
end

--- Inflicts a modifier on the object.
-- @param self Object.
-- @param name Modifier name.
-- @param strength Modifier strength.
Player.inflict_modifier = function(self, name, strength)
	self:send{packet = Packet(packets.MODIFIER_ADD, "string", name, "float", strength)}
	return Actor.inflict_modifier(self, name, strength)
end

--- Called when a modifier is removed.
-- @param self Object.
-- @param name Modifier name.
Player.removed_modifier = function(self, name)
	self:send{packet = Packet(packets.MODIFIER_REMOVE, "string", name)}
	return Actor.removed_modifier(self, name)
end

Player.respawn = function(self)
	self:disable()
	Serialize:save_account(self.account)
end

Player.inventory_cb = function(self, args)
	local id = args.inventory.id
	local funs =
	{
		["inventory-changed"] = function()
			if args.object then
				local name = args.object.spec.name
				local slot = args.inventory:get_slot_by_index(args.index)
				self:send{packet = Packet(packets.INVENTORY_ITEM_ADDED, "uint32", id,
					"uint8", args.index, "string", name, "uint32", args.object.count)}
				if slot then
					self:send{packet = Packet(packets.INVENTORY_ITEM_EQUIPPED, "uint32", id,
						"uint8", args.index, "string", slot)}
				end
			else
				self:send{packet = Packet(packets.INVENTORY_ITEM_REMOVED, "uint32", id,
					"uint8", args.index)}
			end
		end,
		["inventory-equipped"] = function()
			self:send{packet = Packet(packets.INVENTORY_ITEM_EQUIPPED, "uint32", id,
				"uint8", args.index, "string", args.slot)}
		end,
		["inventory-unequipped"] = function()
			self:send{packet = Packet(packets.INVENTORY_ITEM_UNEQUIPPED, "uint32", id,
				"uint8", args.index)}
		end,
		["inventory-subscribed"] = function()
			local owner = Object:find{id = id}
			local spec = owner.spec
			self.inventory_subscriptions[id] = args.inventory
			self:send{packet = Packet(packets.INVENTORY_CREATED, "uint32", id,
				"string", spec.type, "string", spec.name,
				"uint8", args.inventory.size, "bool", id == self.id)}
		end,
		["inventory-unsubscribed"] = function()
			self.inventory_subscriptions[id] = nil
			self:send{packet = Packet(packets.INVENTORY_CLOSED, "uint32", id)}
		end
	}
	local fun = funs[args.type]
	if fun then fun(args) end
end

--- Updates the state of the player.
-- @param self Object.
-- @param secs Seconds since the last update.
Player.update = function(self, secs)
	if self.client then
		-- Check for bugged characters just in case.
		if not self.realized or not self.vision then return self:detach() end
		-- Prevent sectors from unloading if a player is present.
		self:refresh{radius = self.vision.radius}
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
	-- Update the base class.
	Actor.update(self, secs)
end

Player.update_map = function(self)
	-- Discover map markers.
	for k,v in pairs(Marker.dict_discoverable) do
		if (self.position - v.position).length < 3 * self.vision.radius then
			v:unlock()
		end
	end
end

--- Closes unreachable inventories.
-- @param self Object.
Player.update_inventory_subscriptions = function(self)
	if not self.inventory_subscriptions then return end
	for id,inv in pairs(self.inventory_subscriptions) do
		local object = Object:find{id = id}
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
	local packet = Packet(packets.PLAYER_SKILLS)
	for k,v in pairs(self.skills:get_names()) do
		if v then packet:write("string", k) end
	end
	self:send{packet = packet}
end

--- Updates the vision radius of the player.<br/>
-- The vision system needs the direction and position of the player, so we
-- update it here.
-- @param self Object.
Player.update_vision_radius = function(self)
	self.vision.direction = self.rotation * Vector(0,0,-1)
	self.vision.position = self.position
end

Player.vision_cb = function(self, args)
	local funs
	funs =
	{
		["object-animated"] = function(args)
			local o = args.object
			if o.static then return end
			self:send{packet = Packet(packets.OBJECT_ANIMATED, "uint32", o.id,
				"string", args.animation or "", "float", args.time or 0.0)}
		end,
		["object-beheaded"] = function(args)
			local o = args.object
			if o.static then return end
			self:send{packet = Packet(packets.OBJECT_BEHEADED, "uint32", o.id)}
		end,
		["object-dead"] = function(args)
			local o = args.object
			if o.static then return end
			self:send{packet = Packet(packets.OBJECT_DEAD, "uint32", o.id, "bool", args.dead)}
		end,
		["object-dialog"] = function(args)
			local o = args.object
			if args.choices then
				-- Choice.
				local p = {}
				for k,v in ipairs(args.choices) do
					table.insert(p, "string")
					table.insert(p, v)
				end
				self:send{packet = Packet(packets.OBJECT_DIALOG_CHOICE, "uint32", o.id, "bool", o.dialog.user == self, "uint8", #args.choices, unpack(p))}
			elseif args.message then
				-- Message.
				self:send{packet = Packet(packets.OBJECT_DIALOG_MESSAGE, "uint32", o.id, "bool", o.dialog.user == self, "string", args.character or "", "string", args.message)}
			else
				-- None.
				self:send{packet = Packet(packets.OBJECT_DIALOG_NONE, "uint32", o.id)}
			end
		end,
		["object-effect"] = function(args)
			local o = args.object
			self:send{packet = Packet(packets.OBJECT_EFFECT, "uint32", o.id, "string", args.effect)}
		end,
		["object-feat"] = function(args)
			local o = args.object
			if o.static then return end
			self:send{packet = Packet(packets.OBJECT_FEAT, "uint32", o.id, "string", args.anim.name, "uint8", args.move or 0)}
		end,
		["object-hidden"] = function(args)
			local o = args.object
			if o.static then return end
			self:send{packet = Packet(packets.OBJECT_HIDDEN, "uint32", o.id)}
		end,
		["object-moved"] = function(args)
			local o = args.object
			if o.static then return end
			local tilt = (o.tilt and o.tilt.euler[3]) or 0
			local p = Packet(packets.OBJECT_MOVED, "uint32", o.id,
				"float", o.position.x, "float", o.position.y, "float", o.position.z, "float", tilt,
				"float", o.rotation.x, "float", o.rotation.y, "float", o.rotation.z, "float", o.rotation.w,
				"float", o.velocity.x, "float", o.velocity.y, "float", o.velocity.z)
			self:send{packet = p, reliable = false}
		end,
		["object-shown"] = function(args)
			-- Don't send static objects.
			local o = args.object
			if o.static then return end
			if o.spec.name == "static" then return end
			-- Wake up the AI.
			if o.ai then o.ai:refresh() end
			local flags = o.flags or 0
			-- Spec.
			local data_spec = {}
			if o.spec.type ~= "object" then
				data_spec = {
					"string", o.spec.type,
					"string", o.spec.name}
				flags = flags + Protocol.object_show_flags.SPEC
			else
				data_spec = {
					"string", o.model_name or ""}
			end
			-- Actor.
			local data_actor = {}
			if o.spec.type == "actor" then
				data_actor = {
					"bool", o.dead or false,
					"float", (o.tilt and o.tilt.euler[3]) or 0}
				flags = flags + Protocol.object_show_flags.ACTOR
			end
			-- Position.
			local data_pos = {}
			if o.position.x > 1 and o.position.y > 1 and o.position.z > 1 then
				data_pos = {
					"float", o.position.x,
					"float", o.position.y,
					"float", o.position.z}
				flags = flags + Protocol.object_show_flags.POSITION
			end
			-- Rotation.
			local data_rot = {}
			if math.abs(o.rotation.w) < 0.99 then
				data_rot = {
					"float", o.rotation.x,
					"float", o.rotation.y,
					"float", o.rotation.z,
					"float", o.rotation.w}
				flags = flags + Protocol.object_show_flags.ROTATION
			end
			-- Name.
			local data_name = {}
			if o.name then
				data_name = {
					"string", o.name}
				flags = flags + Protocol.object_show_flags.NAME
			end
			-- Animations.
			local data_anims = {}
			if o.animations then
				local num = 0
				for k,v in pairs(o.animations) do
					if v[1] then
						table.insert(data_anims, "string")
						table.insert(data_anims, v[1])
						table.insert(data_anims, "float")
						table.insert(data_anims, Program.time - v[2])
						num = num + 1
					end
				end
				if num > 0 then
					table.insert(data_anims, 1, "uint8")
					table.insert(data_anims, 2, num)
					flags = flags + Protocol.object_show_flags.ANIMS
				end
			end
			-- Equipment.
			local data_slots = {}
			if o.inventory then
				local num = 0
				for slot,index in pairs(o.inventory.equipped) do
					local item = o.inventory:get_object_by_index(index)
					if item then
						table.insert(data_slots, "uint32")
						table.insert(data_slots, index)
						table.insert(data_slots, "string")
						table.insert(data_slots, slot)
						table.insert(data_slots, "string")
						table.insert(data_slots, item.spec.name)
						table.insert(data_slots, "uint32")
						table.insert(data_slots, item.count)
						num = num + 1
					end
				end
				if num > 0 then
					table.insert(data_slots, 1, "uint8")
					table.insert(data_slots, 2, num)
					flags = flags + Protocol.object_show_flags.SLOTS
				end
			end
			-- Stats.
			local data_stats = {}
			if o.stats then
				local num = 0
				for name,stat in pairs(o.stats.stats) do
					if stat.prot == "public" or self == o then
						table.insert(data_stats, "string")
						table.insert(data_stats, name)
						table.insert(data_stats, "int32")
						table.insert(data_stats, math.ceil(stat.value))
						table.insert(data_stats, "int32")
						table.insert(data_stats, math.ceil(stat.maximum))
						num = num + 1
					end
				end
				if num > 0 then
					table.insert(data_stats, 1, "uint8")
					table.insert(data_stats, 2, num)
					flags = flags + Protocol.object_show_flags.SKILLS
				end
			end
			-- Body style.
			local data_body = {}
			if o.body_scale or o.body_style or o.skin_style then
				data_body = {
					"uint8", o.body_scale or 128,
					"uint8", o.body_style and o.body_style[1] or 0,
					"uint8", o.body_style and o.body_style[2] or 0,
					"uint8", o.body_style and o.body_style[3] or 0,
					"uint8", o.body_style and o.body_style[4] or 128,
					"uint8", o.body_style and o.body_style[5] or 0,
					"uint8", o.body_style and o.body_style[6] or 0,
					"uint8", o.body_style and o.body_style[7] or 0,
					"uint8", o.body_style and o.body_style[8] or 255,
					"uint8", o.body_style and o.body_style[9] or 0,
					"string", o.skin_style and o.skin_style[1] or "",
					"uint8", o.skin_style and o.skin_style[2] or 255,
					"uint8", o.skin_style and o.skin_style[3] or 255,
					"uint8", o.skin_style and o.skin_style[4] or 255}
				flags = flags + Protocol.object_show_flags.BODY_STYLE
			end
			-- Head style.
			local data_head = {}
			if o.eye_style or o.face_style or o.hair_style then
				data_head = {
					"string", o.eye_style and o.eye_style[1] or "",
					"uint8", o.eye_style and o.eye_style[2] or 255,
					"uint8", o.eye_style and o.eye_style[3] or 255,
					"uint8", o.eye_style and o.eye_style[4] or 255,
					"uint8", o.face_style and o.face_style[1] or 0,
					"uint8", o.face_style and o.face_style[2] or 0,
					"uint8", o.face_style and o.face_style[3] or 0,
					"uint8", o.face_style and o.face_style[4] or 0,
					"uint8", o.face_style and o.face_style[5] or 0,
					"uint8", o.face_style and o.face_style[6] or 0,
					"uint8", o.face_style and o.face_style[7] or 0,
					"uint8", o.face_style and o.face_style[8] or 0,
					"uint8", o.face_style and o.face_style[9] or 0,
					"uint8", o.face_style and o.face_style[10] or 127,
					"uint8", o.face_style and o.face_style[11] or 127,
					"uint8", o.face_style and o.face_style[12] or 127,
					"uint8", o.face_style and o.face_style[13] or 127,
					"uint8", o.face_style and o.face_style[14] or 0,
					"uint8", o.face_style and o.face_style[15] or 0,
					"string", o.hair_style and o.hair_style[1] or "",
					"uint8", o.hair_style and o.hair_style[2] or 255,
					"uint8", o.hair_style and o.hair_style[3] or 255,
					"uint8", o.hair_style and o.hair_style[4] or 255}
				flags = flags + Protocol.object_show_flags.HEAD_STYLE
			end
			-- Dialog.
			local data_dialog = {}
			if o.dialog and o.dialog.event then
				flags = flags + Protocol.object_show_flags.DIALOG
				local e = o.dialog.event
				if e.choices then
					table.insert(data_dialog, "uint8")
					table.insert(data_dialog, 0)
					table.insert(data_dialog, "uint8")
					table.insert(data_dialog, #e.choices)
					for k,v in ipairs(e.choices) do
						table.insert(data_dialog, "string")
						table.insert(data_dialog, v)
					end
				else
					table.insert(data_dialog, "uint8")
					table.insert(data_dialog, 1)
					table.insert(data_dialog, "string")
					table.insert(data_dialog, e.character or "")
					table.insert(data_dialog, "string")
					table.insert(data_dialog, e.message)
				end
			end
			-- Self.
			if o == self then
				flags = flags + Protocol.object_show_flags.SELF
			end
			-- Count.
			local data_count = {}
			if o.count and o.count > 1 then
				flags = flags + Protocol.object_show_flags.COUNT
					table.insert(data_dialog, "uint32")
					table.insert(data_dialog, o.count)
			end
			-- Send to the player.
			local p = Packet(packets.OBJECT_SHOWN, "uint32", o.id, "uint32", flags)
			p:write(data_spec)
			p:write(data_actor)
			p:write(data_pos)
			p:write(data_rot)
			p:write(data_name)
			p:write(data_anims)
			p:write(data_slots)
			p:write(data_stats)
			p:write(data_body)
			p:write(data_head)
			p:write(data_dialog)
			p:write(data_count)
			self:send(p)
			if o == self then self:update_map() end
		end,
		["object-speech"] = function(args)
			local o = args.object
			self:send{packet = Packet(packets.OBJECT_SPEECH, "uint32", o.id, "string", args.message)}
		end,
		["skill-changed"] = function(args)
			local o = args.object
			local s = o.stats
			if not s then return end
			local v = s:get_skill(args.skill)
			if not v then return end
			if v.prot == "public" or self == o then
				self:send{packet = Packet(packets.OBJECT_SKILL, "uint32", o.id,
					"string", args.skill, "int32", math.ceil(v.value), "int32", math.ceil(v.maximum))}
			end
		end,
		["object-equip"] = function(args)
			-- If the player is subscribed to the inventory of the object, the
			-- equip message is already sent by the inventory listener.
			local id = args.object.id
			if args.object.inventory:is_subscribed(self) then return end
			-- The contents of the inventory slot must be revealed to the client
			-- since it would otherwise have no information on the equipped item.
			self:send{packet = Packet(packets.INVENTORY_ITEM_ADDED,
				"uint32", id, "uint8", args.index, "string", args.item.name, "uint32", args.item.count)}
			-- Send the equip message.
			self:send{packet = Packet(packets.INVENTORY_ITEM_EQUIPPED,
				"uint32", id, "uint8", args.index, "string", args.slot)}
		end,
		["object-unequip"] = function(args)
			-- If the player is subscribed to the inventory of the object, the
			-- unequip message is already sent by the inventory listener.
			local id = args.object.id
			if args.object.inventory:is_subscribed(self) then return end
			-- Send the unequip message.
			self:send{packet = Packet(packets.INVENTORY_ITEM_UNEQUIPPED,
				"uint32", id, "uint8", args.index)}
			-- The client doesn't need the item information of the unsubscribed
			-- inventory anymore so we can clear the item.
			self:send{packet = Packet(packets.INVENTORY_ITEM_REMOVED,
				"uint32", id, "uint8", args.index)}
		end,
		["voxel-block-changed"] = function(args)
			self:send{packet = Voxel:get_block{index = args.index, type = packets.VOXEL_DIFF}}
		end,
		["world-effect"] = function(args)
			self:send{packet = Packet(packets.EFFECT_WORLD, "string", args.effect,
				"float", args.point.x, "float", args.point.y, "float", args.point.z)}
		end
	}
	local fun = funs[args.type]
	if fun then fun(args) end
end

--- Writes the object to a database.
--
-- Player objects are not added to sectors unless they are corpses. Apart from
-- that and the account information being written, they behave similar to
-- ordinary actors.
--
-- @param self Object.
-- @param db Database.
Player.write_db = function(self, db)
	-- Write the object.
	local data = serialize{
		angular = self.angular,
		body_scale = self.body_scale,
		body_style = self.body_style,
		eye_style = self.eye_style,
		face_style = self.face_style,
		hair_style = self.hair_style,
		name = self.name,
		physics = self.dead and "rigid" or "kinematic",
		position = self.position,
		rotation = self.rotation,
		skin_style = self.skin_style}
	db:query([[REPLACE INTO object_data (id,type,spec,dead,data) VALUES (?,?,?,?,?);]],
		{self.id, "player", self.spec.name, self.dead and 1 or 0, data})
	-- Write the sector.
	if self.sector and not self.client then
		db:query([[REPLACE INTO object_sectors (id,sector,time) VALUES (?,?,?);]], {self.id, self.sector, 0})
	else
		db:query([[DELETE FROM object_sectors where id=?;]], {self.id})
	end
	-- Write the inventory contents.
	db:query([[DELETE FROM object_inventory WHERE parent=?;]], {self.id})
	for index,object in pairs(self.inventory.stored) do
		object:write_db(db, index)
	end
	-- Write skills.
	db:query([[DELETE FROM object_skills WHERE id=?;]], {self.id})
	for name,value in pairs(self.skills.skills) do
		db:query([[REPLACE INTO object_skills (id,name) VALUES (?,?);]], {self.id, name})
	end
	-- Write stats.
	db:query([[DELETE FROM object_stats WHERE id=?;]], {self.id})
	for name,args in pairs(self.stats.stats) do
		db:query([[REPLACE INTO object_stats (id,name,value) VALUES (?,?,?);]], {self.id, name, args.value})
	end
	-- Write account information.
	if self.client and self.account then
		Serialize:save_account(self.account, self)
	end
end
