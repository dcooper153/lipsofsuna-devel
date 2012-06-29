-- Automatically equips an item.
Actionspec{name = "autoequip", type = "item", func = function(item, user)
	if not item.spec.equipment_slot then return end
	-- Find the object whose inventory contains the item.
	local owner = ServerObject:find{id = item.parent}
	if not owner then return end
	-- Check if the item is equipped.
	local index = owner.inventory:get_slot_by_object(item)
	if not index then return end
	local slot = owner.inventory:get_slot_by_index(index)
	-- Toggle equip status.
	if slot then
		owner.inventory:unequip_index(index)
	else
		owner.inventory:equip_index(index, item.spec_equipment_slot)
	end
end}
