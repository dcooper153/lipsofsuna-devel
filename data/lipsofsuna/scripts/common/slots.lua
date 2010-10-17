--- Create and synchronize visible equipment slots.
-- @name Slots
-- @class table
Slots = Class()
Slots.objectdict = {}
Slots.ownerdict = {}
setmetatable(Slots.objectdict, {__mode = "kv"})
setmetatable(Slots.ownerdict, {__mode = "k"})

--- Clears and unregisters all slots.
-- @param self Slots.
Slots.clear = function(self)
	local empty = true
	for k,v in pairs(self.slots) do
		if v.object then self:set_object{slot = k} end
		empty = false
	end
	if not empty then
		self.slots = {}
		Vision:event{type = "slots-cleared", object = self.owner}
	end
end

--- Finds slots by owner or stored object.
-- @param clss Slots class.
-- @param args Arguments.<ul>
--   <li>owner: Owner object.</li>
--   <li>object: Stored object.</li></ul>
-- @return Slots or nil.
Slots.find = function(clss, args)
	if args.owner then
		return clss.ownerdict[args.owner]
	elseif args.object then
		return clss.objectdict[args.object]
	end
end

--- Creates a new slots object.
-- @param clss Slots class.
-- @param args Arguments.
-- @return New slots.
Slots.new = function(clss, args)
	local s = Class.new(clss, args)
	s.slots = {}
	if s.owner then
		if clss.ownerdict[s.owner] then error("object already has slots") end
		clss.ownerdict[s.owner] = s
	end
	return s
end

--- Gets an object in a slot.
-- @param self Slots.
-- @param args Arguments.<ul>
--   <li>slot: Slot name.</li></ul>
-- @return Object or nil.
Slots.get_object = function(self, args)
	local v = args.slot and self.slots[args.slot]
	return (v and v.object) or nil
end

--- Gets information on a slot.
-- @param self Slots.
-- @param args Arguments.<ul>
--   <li>slot: Slot name.</li></ul>
-- @return Slot information or nil.
Slots.get_slot = function(self, args)
	local v = args.slot and self.slots[args.slot]
	if not v then return end
	return {name = v.name, node = v.node, object = v.object, prot = v.prot}
end

--- Registers a slot.
-- @param self Slots.
-- @param args Arguments.<ul>
--   <li>name: Slot name. (required)</li>
--   <li>node: Armature node name.</li>
--   <li>object: Object to store.</li>
--   <li>prot: Protection type ("public"/"private")</li></ul>
Slots.register = function(self, args)
	if not args.name then return end
	if self.slots[args.name] then error("slots already exists") end
	local v =
	{
		name = args.name,
		node = args.node or "#root",
		object = args.object,
		prot = args.prot or "private"
	}
	self.slots[args.name] = v
	Vision:event{type = "slot-changed", object = self.owner, slot = args.name}
	return v
end

--- Sets the object in a slot.
-- @param self Slots.
-- @param args Arguments.<ul>
--   <li>slot: Slot name.</li>
--   <li>object: Object.</li></ul>
Slots.set_object = function(self, args)
	if not args.slot then return end
	local v = self.slots[args.slot]
	if not v then return end
	-- Maintain parent list.
	if args.object and Slots.objectdict[args.object] then error("object already in slots") end
	if v.object then Slots.objectdict[v.object] = nil end
	if args.object then Slots.objectdict[args.object] = self end
	-- Add to slot.
	v.object = args.object
	Vision:event{type = "slot-changed", object = self.owner, slot = args.slot}
end

--- Sends a new vision event for a slot.
-- @param self Slots.
-- @param args Arguments.<ul>
--   <li>slot: Slot name.</li></ul>
Slots.update_slot = function(self, args)
	if not args.slot then return end
	Vision:event{type = "slot-changed", object = self.owner, slot = args.slot}
end
