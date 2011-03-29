require "common/inventory"

Slots = Class(Inventory)

--- Sets an equipment item.
-- @param self Slots.
-- @param args Arguments.<ul>
--   <li>model: Model name.</li>
--   <li>slot: Slot name.</li></ul>
Slots.set_object = function(self, args)
	-- Delete old item.
	local object = self:get_object(args)
	if object then
		object:detach()
	end
	-- Create new item.
	if args.model then
		object = Object{model = args.model, collision_group = Physics.GROUP_EQUIPMENT, spec = args.spec}
		object:update_model(args)
		self.slots[args.slot] = object
	else
		self.slots[args.slot] = nil
	end
end

Slots.update = function(self)
	local species = Species:find{name = self.owner.race}
	for slot,object in pairs(self.slots) do
		local slot = species and species.equipment_slots[slot]
		if slot and slot.node and self.owner.realized then
			-- Show slot.
			local p,r = self.owner:find_node{name = slot.node, space = "world"}
			local h = object:find_node{name = "#handle"}
			if h then p = p - r * h end
			object.position = p
			object.rotation = r
			object.realized = true
		else
			-- Hide slot.
			object:detach()
		end
	end
end
