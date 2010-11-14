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

	local inventory = self.inventory
	inventory:subscribe{object = self, callback = function(args) self:inventory_cb(args) end}
	self:add_item{object = Item:create{name = "axe"}}
	for i = 1,10 do self:add_item{object = Item:create{category = "potion"}} end
	for i = 1,50 do self:add_item{object = Item:create{category = "reagent"}} end
	self:add_item{object = Item:create{name = "mortar and pestle"}}
	self:add_item{object = Item:create{name = "saw"}}

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
		self:effect{effect = "thud-000"}
		self:damaged(damage)
		Particles:debug(result.point, "test")
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
				local name = args.object:get_name_with_count()
				self:send{packet = Packet(packets.INVENTORY_ITEM_ADDED, "uint32", id,
					"uint8", args.slot, "uint32", 1, "string", name)}
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
		local slots = Slots:find{owner = o}
		local skills = Skills:find{owner = o}
		for k,v in pairs(o.animations) do
			local a = o:get_animation{channel = k}
			funs["object-animated"]({animation = v, channel = k, object = o, permanent = true, time = a.time, weight = a.weight})
		end
		if slots then
			for k,v in pairs(slots.slots) do
				funs["slot-changed"]({object = o, slot = k})
			end
		end
		if skills then
			for k,v in pairs(skills.skills) do
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
			self:send{packet = Packet(packets.OBJECT_MOVED, "uint32", o.id,
				"float", o.position.x, "float", o.position.y, "float", o.position.z,
				"float", o.rotation.x, "float", o.rotation.y, "float", o.rotation.z, "float", o.rotation.w)}
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
					"string", o.hair_style or "",
					"string", o.hair_color or "white",
					"float", o.body_scale or 1,
					"float", o.nose_scale or 1,
					"float", o.bust_scale or 1)
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
			local s = Slots:find{owner = o}
			if not s then return end
			local v = s:get_slot{slot = args.slot}
			if not v then return end
			if v.prot == "public" or self == o then
				local model = v.object and v.object.model_name or ""
				local node = v.node or ""
				local name = v.object and v.object:get_name_with_count() or ""
				self:send{packet = Packet(packets.OBJECT_SLOT, "uint32", o.id,
					"string", v.name or "", 
					"string", node,
					"string", model,
					"string", name)}
			end
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
