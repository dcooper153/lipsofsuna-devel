Container = Class(Object)

--- Creates a container.
-- @param clss Container class.
-- @param args Arguments.
-- @return New container.
Container.new = function(clss, args)
	local self = Object.new(clss, args)
	self.name = self.name or "container"
	self.physics = "rigid"
	self.inventory = Inventory{owner = self, size = 20}
	return self
end

--- Fills the container with random loot.
-- @param self Object.
Container.create_random_loot = function(self)
	for i = 1,self.inventory.size do
		self.inventory:set_object{slot = i, object = Item:create()}
	end
end

--- Called when the object is used.
-- @param self Object.
-- @param user User.
Container.use_cb = function(self, user)
	local inv = Inventory:find{owner = self}
	self:animate{animation = "opening", weight = 100}
	self:animate{animation = "open", channel = 1, weight = 1.0, permanent = true}
	self:say("Looted")
	if user.inventory_cb then
		inv:subscribe{object = user, callback = function(args) user:inventory_cb(args) end}
	end
	Object.use_cb(self, user)
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Container.write = function(self)
	local inv = Inventory:find{owner = self}
	return string.gsub(Object.write(self), "}\n",
		"}\n" .. serialize_inventory(inv))
end
