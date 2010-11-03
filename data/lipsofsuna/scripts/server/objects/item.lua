Item = Class(Object)
Item.pickable = true

Item.setter = function(self, key, value)
	if key == "itemspec" then
		local spec = type(value) == "string" and Itemspec:find{name = value} or value
		if not spec then return end
		Object.setter(self, key, spec)
		self.mass = spec.mass
		self.model = spec.model
		self.name = spec.name
	else
		Object.setter(self, key, value)
	end
end

--- Creates a copy of the item.
-- @param self Object.
-- @return Object.
Item.clone = function(self)
	return Item{
		itemspec = self.itemspec,
		angular = self.angular,
		position = self.position,
		rotation = self.rotation}
end

--- Causes the item to take damage.
-- @param self Object.
-- @param amount Amount of damage.
Item.damaged = function(self, amount)
	-- Items whose spec has no health are indestructible.
	if not self.itemspec.health then return end
	-- Subtract health.
	local h = self.health or self.itemspec.health
	self.health = math.max(0, h - amount)
	-- Destroy when out of health.
	if self.health == 0 then
		self:die()
	end
end

--- Destroys the item.
-- @param self Object.
Item.die = function(self)
	-- Mark as dead.
	if self.dead then return end
	self.dead = true
	-- Execute destruction actions.
	for k,v in ipairs(self.itemspec.destroy_actions) do
		if v == "explode" then
			for k,v in pairs(Object:find{point = self.position, radius = 5}) do
				Particles:create(self.position, "explosion1")
				v:damaged(40)
			end
		end
	end
	-- Remove from the world.
	Object.die(self)
end

--- Splits items from the stack.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>count: Number of items to split.</li></ul>
-- @return Object.
Item.split = function(self, args)
	local c = args and args.count or 1
	if c < self:get_count() then
		local o = self:clone()
		self:subtract{count = c}
		o.count = c
		return o
	end
	return self
end

--- Creates a random or a named item.
-- @param clss Item class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li>
--   <li>name: Item name.</li></ul>
-- @return Object or nil.
Item.create = function(clss, args)
	local self
	-- Select item type.
	local spec = args and args.name and Itemspec:find(args) or Itemspec:random(args)
 	if not spec then return end
	-- Create new object.
	self = Item:new(args)
	-- TODO
	self.category = nil
	self.itemspec = spec
	return self
end

--- Creates an item.
-- @param clss Item class.
-- @param args Arguments.
-- @return New item.
Item.new = function(clss, args)
	local self = Object.new(clss, args)
	self.physics = args and args.physics or "rigid"
	return self
end

--- Fires or throws the item.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>collision: Trigger at collision.</li>
--   <li>owner: Object firing the projectile. (required)</li>
--   <li>point: Firing point relative to the owner.</li>
--   <li>speed: Initial speed.</li>
--   <li>timer: Trigger at timeout.</li></ul>
Item.fire = function(self, args)
	if args.owner then
		local pt = args.point or Vector()
		local sp = args.speed or 20
		local user = args.owner
		local proj = self:split()
		if args.collision then
			proj.contact_cb = function(self, result)
				if result.object == proj.owner then return end
				Combat:apply_ranged_hit{
					attacker = user,
					feat = Feat:find{name = "attack"},
					point = result.point,
					projectile = proj,
					target = result.object,
					tile = result.tile}
			end
		end
		if args.timer then
			Timer{delay = args.timer, func = function(timer)
				if proj.realized then
					Combat:apply_ranged_hit{
						attacker = user,
						feat = Feat:find{name = "attack"},
						point = proj.position,
						projectile = proj}
				end
				timer:disable()
			end}
		end
		proj:detach()
		proj.owner = args.owner
		proj.position = args.owner.position + args.owner.rotation * pt
		proj.rotation = args.owner.rotation
		proj.velocity = args.owner.rotation * Vector(0, 0, -sp)
		proj.save = false
		proj.realized = true
	end
end

--- Called when the object is examined.
-- @param self Object.
-- @param user User.
Item.examine_cb = function(self, user)
	user:send{packet = Packet(packets.MESSAGE, "string", self.itemspec.name)}
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Item.use_cb = function(self, user)
	if self.itemspec.categories["tool"] then
		Crafting:send{user = user}
	elseif self.itemspec.categories["book"] then
		user:send{packet = Packet(packets.BOOK,
			"string", self.itemspec.name,
			"string", self.itemspec.book_text)}
	elseif self.itemspec.categories["potion"] then
		local types =
		{
			["lesser health potion"] = { skill = "health", value = 10 },
			["health potion"] = { skill = "health", value = 30 },
			["greater health potion"] = { skill = "health", value = 60 },
			["lesser mana potion"] = { skill = "willpower", value = 10 },
			["mana potion"] = { skill = "willpower", value = 30 },
			["greater mana potion"] = { skill = "willpower", value = 60 }
		}
		local type = types[self.name]
		if not type then return end
		local skills = user.skills
		if not skills then return end
		local value = skills:get_value{skill = type.skill}
		if not value then return end
		skills:set_value{skill = type.skill, value = value + type.value}
		user:effect{effect = "spell-000"}
		self:subtract{count = 1}
	end
end

--- Called when the item is being equipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
Item.equipped = function(self, user, slot)
	if self.itemspec.categories["shield"] then
		user:animate{animation = "hold-left", channel = Animation.CHANNEL_EQUIP_LEFT, weight = 10.0, permanent = true}
	end
end

--- Called when the item is being unequipped.
-- @param self Object.
-- @param user User object.
-- @param slot Slot name.
Item.unequipped = function(self, user, slot)
	if self.itemspec.categories["shield"] then
		user:animate{channel = Animation.CHANNEL_EQUIP_LEFT}
	end
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Item.write = function(self)
	return "local self=Item{" ..
		"angular=" .. serialize_value(self.angular) .. "," ..
		"id=" .. serialize_value(self.id) .. "," ..
		"itemspec=" .. serialize_value(self.itemspec.name) .. "," ..
		"position=" .. serialize_value(self.position) .. "," ..
		"rotation=" .. serialize_value(self.rotation) .. "}\n" ..
		"return self"
end
