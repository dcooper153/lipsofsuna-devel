require "common/inventory"

Slots = Class(Inventory)

--- Sets an equipment item.
-- @param self Slots.
-- @param args Arguments.<ul>
--   <li>model: Model name.</li>
--   <li>slot: Slot name.</li></ul>
Slots.set_object = function(self, args)
	local object = self:get_object(args)
	if not args.model then
		-- Clear item.
		if object then object.realized = false end
		Inventory.set_object(self, args)
	elseif object then
		-- Replace item.
		object.model = args.model
		object:update_model(args)
	else
		-- Create item.
		object = Object{model = args.model}
		object:update_model(args)
		Inventory.set_object(self, {slot = args.slot, object = object})
	end
end

Slots.update = function(self)
	local species = Species:find{name = self.owner.race}
	for slot,object in pairs(self.slots) do
		local slot = species and species.equipment_slots[slot]
		if slot and slot.node and self.owner.realized then
			-- Show slot.
			local p,r = self.owner:find_node{name = slot.node, space = "world"}
			object.position = p
			object.rotation = r
			object.realized = true
		else
			-- Hide slot.
			object.realized = false
		end
	end
end

Eventhandler{type = "tick", func = function(self, args)
	for k,v in pairs(Slots.dict_owner) do
		v:update()
	end
end}
