Player = Class(Creature)
Player.clients = {}
local findfunc = Player.find

Player.find = function(clss, args)
	if args and args.client then
		return clss.clients[args.client]
	end
	return findfunc(clss, args)
end

--- Creates a new player object.
-- @param clss Player class.
-- @param args Arguments.<ul>
--   <li>account: Account data.</li>
--   <li>angular: Angular velocity.</li>
--   <li>body_scale: Scale factor of the body.</li>
--   <li>bust_scale: Scale factor of the bust.</li>
--   <li>client: Client controlling the character.</li>
--   <li>eye_style: Eye style defined by an array of {style, red, green, blue}.</li>
--   <li>hair_style: Hair style defined by an array of {style, red, green, blue}.</li>
--   <li>id: Unique object ID or nil for a random free one.</li>
--   <li>jumped: Jump timer.</li>
--   <li>name: Name of the creature.</li>
--   <li>nose_scale: Scale factor of the nose.</li>
--   <li>physics: Physics mode.</li>
--   <li>position: Position vector of the creature.</li>
--   <li>rotation: Rotation quaternion of the creature.</li>
--   <li>skin_style: Skin style defined by an array of {style, red, green, blue}.</li>
--   <li>spec: Species of the creature.</li>
--   <li>realized: True to add the object to the simulation.</li></ul>
-- @return Player.
Player.new = function(clss, args)
	local self = Creature.new(clss, args)
	self.account = args.account
	if args.client then self:set_client(args.client) end
	return self
end

--[[Player.contact_cb = function(self, result)
	local damage = Damage:impulse(result.impulse)
	local resist = Voxel:player_contact(self, result.point)
	if damage > resist then
		damage = damage - resist
		Effect:play{effect = "impact1", point = result.point}
		self:damaged(damage)
	end
end--]]

Player.set_client = function(self, client)
	self.client = client
	self.vision = Vision{object = self, radius = 10, callback = function(args) self:vision_cb(args) end}
	self.vision.terrain = {}
	self.inventory:subscribe{object = self, callback = function(args) self:inventory_cb(args) end}
	-- Terrain listener.
	self.player_timer = Timer{delay = 0.2, func = function()
		self:update_vision_radius()
		self:vision_cb{type = "player-tick"}
	end}
end

--- Causes the player to die and respawn.
-- @param self Player.
Player.die = function(self)
	if Creature.die(self) then
		self:send{packet = Packet(packets.MESSAGE, "string", "You have died...")}
		self.contact_cb = false
	else
		self:send{packet = Packet(packets.MESSAGE, "string", "Saved by Sanctuary.")}
	end
end

Player.detach = function(self, keep)
	self.player_timer:disable()
	self.vision:disable()
	self.realized = false
	if not keep then
		self:send{packet = Packet(packets.CHARACTER_CREATE)}
		Player.clients[self.client] = nil
	end
end

--- Inflicts a modifier on the object.
-- @param self Object.
-- @param name Modifier name.
-- @param strength Modifier strength.
Player.inflict_modifier = function(self, name, strength)
	self:send{packet = Packet(packets.MODIFIER_ADD, "string", name, "float", strength)}
	return Creature.inflict_modifier(self, name, strength)
end

--- Called when a modifier is removed.
-- @param self Object.
-- @param name Modifier name.
Player.removed_modifier = function(self, name)
	self:send{packet = Packet(packets.MODIFIER_REMOVE, "string", name)}
	return Creature.removed_modifier(self, name)
end

Player.respawn = function(self)
	self:detach()
	Serialize:save_account(self.account)
end

Player.inventory_cb = function(self, args)
	local id = args.inventory.owner.id
	local funs =
	{
		["inventory-changed"] = function()
			local islot = (type(args.slot) == "number") and args.slot or 0
			local sslot = (type(args.slot) == "string") and args.slot or ""
			if args.object then
				local name = args.object.spec.name
				self:send{packet = Packet(packets.INVENTORY_ITEM_ADDED, "uint32", id,
					"uint8", islot, "string", sslot, "uint32", args.object.count, "string", name)}
			else
				self:send{packet = Packet(packets.INVENTORY_ITEM_REMOVED, "uint32", id,
					"uint8", islot, "string", sslot)}
			end
		end,
		["inventory-subscribed"] = function()
			local spec = args.inventory.owner.spec
			self:send{packet = Packet(packets.INVENTORY_CREATED, "uint32", id,
				"string", spec.type, "string", spec.name,
				"uint8", args.inventory.size, "bool", args.inventory.owner == self)}
		end,
		["inventory-unsubscribed"] = function()
			self:send{packet = Packet(packets.INVENTORY_CLOSED, "uint32", id)}
		end
	}
	local fun = funs[args.type]
	if fun then fun(args) end
end

--- Updates the vision radius of the player.<br/>
-- The view distance depends on the perception skill so it's necessary to
-- recalculate it occasionally. That can be done by calling this function.
-- @param self Object.
Player.update_vision_radius = function(self)
	local skills = self.skills
	local perception = skills:get_value{skill = "perception"}
	if not perception then return end
	local r = 15 + perception / 6
	if math.floor(r) ~= self.vision.radius then
		self.vision.radius = r
		self.vision:rescan_objects()
	end
end

Player.vision_cb = function(self, args)
	local funs
	local sendinfo = function(o)
		if o.animations then
			for k,v in pairs(o.animations) do
				funs["object-animated"]({animation = v[1], channel = k, object = o, permanent = true, repeat_start = v[4], time = Program.time - v[3], weight = v[2]})
			end
		end
		if o.inventory then
			for k,v in pairs(o.inventory.slots) do
				if type(k) == "string" then
					funs["slot-changed"]({object = o, slot = k})
				end
			end
		end
		if o.skills then
			for k,v in pairs(o.skills.skills) do
				funs["skill-changed"]({object = o, skill = k})
			end
		end
	end
	local sendmap = function()
		local b = Voxel:find_blocks{point = self.position, radius = self.vision.radius}
		for k,v in pairs(b) do
			funs["voxel-block-changed"]({index = k, stamp = v})
		end
	end
	local sendself = function()
		self:send{packet = Packet(packets.OBJECT_SELF, "uint32", self.id)}
	end
	funs =
	{
		["object-animated"] = function(args)
			local o = args.object
			self:send{packet = Packet(packets.OBJECT_ANIMATED, "uint32", o.id,
				"string", args.animation or "", "uint8", args.channel or 255,
				"bool", args.permanent or false, "float", args.time or 0.0,
				"float", args.weight or 1.0, "float", args.repeat_start or 0.0)}
		end,
		["object-dead"] = function(args)
			local o = args.object
			self:send{packet = Packet(packets.OBJECT_DEAD, "uint32", o.id, "bool", args.dead)}
		end,
		["object-effect"] = function(args)
			local o = args.object
			self:send{packet = Packet(packets.OBJECT_EFFECT, "uint32", o.id, "string", args.effect)}
		end,
		["object-feat"] = function(args)
			local o = args.object
			self:send{packet = Packet(packets.OBJECT_FEAT, "uint32", o.id, "string", args.anim.name)}
		end,
		["object-hidden"] = function(args)
			local o = args.object
			self:send{packet = Packet(packets.OBJECT_HIDDEN, "uint32", o.id)}
		end,
		["object-moved"] = function(args)
			local o = args.object
			local p = Packet(packets.OBJECT_MOVED, "uint32", o.id,
				"float", o.position.x, "float", o.position.y, "float", o.position.z,
				"float", o.rotation.x, "float", o.rotation.y, "float", o.rotation.z, "float", o.rotation.w,
				"float", o.velocity.x, "float", o.velocity.y, "float", o.velocity.z)
			self:send{packet = p, reliable = false}
			if o == self then
				self.vision:rescan_objects()
				sendmap()
			end
		end,
		["object-shown"] = function(args)
			local o = args.object
			-- Append basic data.
			local p = Packet(packets.OBJECT_SHOWN, "uint32", o.id,
				"string", o.spec.type, "string", o.spec.name or "",
				"string", o.model_name, "string", o.name or "",
				"float", o.position.x, "float", o.position.y, "float", o.position.z,
				"float", o.rotation.x, "float", o.rotation.y, "float", o.rotation.z, "float", o.rotation.w)
			-- Append optional customizations.
			if o.spec.type == "species" and o.spec.models then
				p:write("bool", o.dead or false,
					"float", o.body_scale or 1,
					"float", o.nose_scale or 1,
					"float", o.bust_scale or 1,
					"string", o.eye_style and o.eye_style[1] or "",
					"uint8", o.eye_style and o.eye_style[2] or 255,
					"uint8", o.eye_style and o.eye_style[3] or 255,
					"uint8", o.eye_style and o.eye_style[4] or 255,
					"string", o.hair_style and o.hair_style[1] or "",
					"uint8", o.hair_style and o.hair_style[2] or 255,
					"uint8", o.hair_style and o.hair_style[3] or 255,
					"uint8", o.hair_style and o.hair_style[4] or 255,
					"string", o.skin_style and o.skin_style[1] or "",
					"uint8", o.skin_style and o.skin_style[2] or 255,
					"uint8", o.skin_style and o.skin_style[3] or 255,
					"uint8", o.skin_style and o.skin_style[4] or 255)
			end
			-- Send to the player.
			self:send{packet = p}
			if o == self then sendself() end
			sendinfo(o)
			sendmap()
		end,
		["object-speech"] = function(args)
			local o = args.object
			self:send{packet = Packet(packets.OBJECT_SPEECH, "uint32", o.id, "string", args.message)}
		end,
		["particle-ray"] = function(args)
			self:send{packet = Packet(packets.PARTICLE_RAY,
				"float", args.src.x, "float", args.src.y, "float", args.src.z,
				"float", args.dst.x, "float", args.dst.y, "float", args.dst.z,
				"float", args.life)}
		end,
		["player-tick"] = function(args)
			sendmap()
		end,
		["skill-changed"] = function(args)
			local o = args.object
			local s = Skills:find{owner = o}
			if not s then return end
			local v = s:get_skill{skill = args.skill}
			if not v then return end
			if v.prot == "public" or self == o then
				self:send{packet = Packet(packets.OBJECT_SKILL, "uint32", o.id,
					"string", args.skill, "int32", math.ceil(v.value), "int32", math.ceil(v.maximum))}
			end
		end,
		["slot-changed"] = function(args)
			local o = args.object
			local item = o:get_item{slot = args.slot}
			local spec = item and item.spec.name
			local model = item and item.model_name or ""
			local count = item and item.count or 1
			self:send{packet = Packet(packets.OBJECT_SLOT,
				"uint32", o.id, "uint32", count,
				"string", spec, "string", args.slot)}
		end,
		["voxel-block-changed"] = function(args)
			if self.vision.terrain[args.index] ~= args.stamp then
				self:send{packet = Voxel:get_block{index = args.index, type = packets.VOXEL_DIFF}}
				self.vision.terrain[args.index] = args.stamp
			end
		end,
		["world-effect"] = function(args)
			self:send{packet = Packet(packets.WORLD_EFFECT, "string", args.effect,
				"float", args.point.x, "float", args.point.y, "float", args.point.z)}
		end
	}
	local fun = funs[args.type]
	if fun then fun(args) end
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Player.write = function(self)
	return string.format("local self=Player%s\n%s%s%s", serialize{
		angular = self.angular,
		body_scale = self.body_scale,
		bust_scale = self.bust_scale,
		dead = self.dead,
		eye_style = self.eye_style,
		hair_style = self.hair_style,
		name = self.name,
		nose_scale = self.nose_scale,
		physics = self.dead and "rigid" or "kinematic",
		position = self.position,
		rotation = self.rotation,
		skin_style = self.skin_style,
		spec = self.spec.name},
		Serialize:encode_skills(self.skills),
		Serialize:encode_inventory(self.inventory),
		"return self")
end

------------------------------------------------------------------------------

Eventhandler{type = "login", func = function(self, event)
	print("Client login")
	Network:send{client = event.client, packet = Packet(packets.CLIENT_AUTHENTICATE)}
end}
Eventhandler{type = "logout", func = function(self, event)
	print("Client logout")
	-- Detach the player object.
	local object = Player.clients[event.client]
	if object then
		object:detach()
		Player.clients[event.client] = nil
	end
	-- Update the account.
	local account = Account.dict_client[event.client]
	if account then
		Serialize:save_account(account, object)
		Account.dict_client[event.client] = nil
	end
end}
