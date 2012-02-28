require "common/inventory"

Slots = Class(Inventory)

--[[
--- Sets an equipment item.
-- @param self Slots.
-- @param args Arguments.<ul>
--   <li>model: Model name.</li>
--   <li>slot: Slot name.</li></ul>
Slots.set_object = function(self, index, object)
	-- Delete the old item.
	local oldobj = self:get_object_by_index(index)
	if oldobj then
		oldobj:detach()
	end
	-- Create the new item.
	if args.model then
		object = Item{collision_group = Physics.GROUP_EQUIPMENT, spec = args.spec}
		object:set_model()
		self.slots[args.slot] = object
	else
		self.slots[args.slot] = nil
	end
end

Slots.equip_index = function(self, index, slot)
	Inventory.equip_index(index, slot)
	local object = self:get_object_by_slot(slot)
	if not object then return end
	object:set_model()
end
]]
