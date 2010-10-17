Slots = Class()
Slots.ownerdict = {}
setmetatable(Slots.ownerdict, {__mode = "k"})

Slots.find = function(clss, args)
	return Slots.ownerdict[args.owner]
end

Slots.new = function(clss, args)
	local self = Class.new(clss, args)
	self.slots = {}
	clss.ownerdict[args.owner] = self
	return self
end

Slots.set_object = function(self, args)
	if not args.model then
		-- Clear the slot.
		if self.slots[args.node] then
			self.slots[args.node].realized = false
		end
		self.slots[args.node] = nil
	else
		-- Set the slot.
		if self.slots[args.node] then
			self.slots[args.node].model = args.model
		else
			self.slots[args.node] = Object{model = args.model}
		end
		-- Update effects.
		local obj = self.slots[args.node]
		obj:update_model(args)
	end
end

Slots.update = function(self)
	if self.owner.realized then
		-- Show slots.
		for node,slot in pairs(self.slots) do
			local p,r = self.owner:find_node{name = node, space = "world"}
			if p and self.owner.realized then
				slot.position = p
				slot.rotation = r
				slot.realized = true
			else
				slot.realized = false
			end
		end
	else
		-- Hide slots.
		for node,slot in pairs(self.slots) do
			slot.realized = false
		end
	end
end

Eventhandler{type = "tick", func = function(self, args)
	for k,v in pairs(Slots.ownerdict) do
		v:update()
	end
end}
