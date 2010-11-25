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
-- @param args Arguments.
-- @return Player.
Player.new = function(clss, args)
	local self = Creature.new(clss, args)
	self.species = args.species or "aer-player"
	self.name = args and args.name or Names:random()
	self.vision = Vision{object = self, radius = 10, callback = function(args) self:vision_cb(args) end}
	self.vision.terrain = {}
	self.inventory:subscribe{object = self, callback = function(args) self:inventory_cb(args) end}

	-- Terrain listener.
	self.player_timer = Timer{delay = 0.2, func = function()
		self:update_vision_radius()
		self:vision_cb{type = "player-tick"}
	end}

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

--- Causes the player to die and respawn.
-- @param self Player.
Player.die = function(self)
	self:send{packet = Packet(packets.MESSAGE, "string", "You have died...")}
	Creature.die(self)
	self.contact_cb = false
end

Player.respawn = function(self)
	self.player_timer:disable()
	self.vision:disable()
	self:send{packet = Packet(packets.CHARACTER_CREATE)}
	Player.clients[self.client] = nil
end

Player.inventory_cb = function(self, args)
	local id = args.inventory.owner.id
	local funs =
	{
		["inventory-changed"] = function()
			if args.object then
				local name = args.object.itemspec and args.object.itemspec.name
				self:send{packet = Packet(packets.INVENTORY_ITEM_ADDED, "uint32", id,
					"uint8", args.slot, "uint32", args.object:get_count(), "string", name)}
			else
				self:send{packet = Packet(packets.INVENTORY_ITEM_REMOVED, "uint32", id,
					"uint8", args.slot)}
			end
		end,
		["inventory-subscribed"] = function()
			self:send{packet = Packet(packets.INVENTORY_CREATED, "uint32", id,
				"uint8", args.inventory.size, "bool", args.inventory.owner == self)}
		end,
		["inventory-unsubscribed"] = function()
			user:send{packet = Packet(packets.INVENTORY_CLOSED, "uint32", id)}
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
		for k,v in pairs(o.animations) do
			local a = o:get_animation{channel = k}
			funs["object-animated"]({animation = v, channel = k, object = o, permanent = true, time = a.time, weight = a.weight})
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
				"float", args.weight or 1.0)}
		end,
		["object-effect"] = function(args)
			local o = args.object
			self:send{packet = Packet(packets.OBJECT_EFFECT, "uint32", o.id, "string", args.effect)}
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
			local t = (o.species and 0) or (o.itemspec and 1) or (o.spec and 2) or 3
			-- Append basic data.
			local p = Packet(packets.OBJECT_SHOWN, "uint32", o.id,
				"uint8", t, "string", o.model_name, "string", o.name or "",
				"float", o.position.x, "float", o.position.y, "float", o.position.z,
				"float", o.rotation.x, "float", o.rotation.y, "float", o.rotation.z, "float", o.rotation.w)
			-- Append optional customizations.
			if o.species and o.species.models then
				p:write("string", o.species.name,
					"string", o.gender or "female",
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
			local spec = item and item.itemspec and item.itemspec.name or ""
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

------------------------------------------------------------------------------

Eventhandler{type = "login", func = function(self, event)
	print("Client login")
end}
Eventhandler{type = "logout", func = function(self, event)
	print("Client logout")
	local object = Player.clients[event.client]
	if not object then return end
	Player.clients[event.client] = nil
	object.realized = false
	object.player_timer:disable()
	object.vision:disable()
end}
