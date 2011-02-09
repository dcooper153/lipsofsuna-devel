--- Moves an inventory item to another inventory.
-- @param clss Actions class.
-- @param user Object performing the action.
-- @param srcid Source inventory ID.
-- @param srcslot Source inventory or equipment slot.
-- @param dstid Destination inventory ID.
-- @param dstslot Destination inventory or equipment slot.
-- @return True on success.
Actions.move_from_inv_to_inv = function(clss, user, srcid, srcslot, dstid, dstslot)
	-- Find inventories.
	local srcinv = Inventory:find{id = srcid}
	if not srcinv or not srcinv:subscribed{object = user} then return end
	local dstinv = Inventory:find{id = dstid}
	if not dstinv or not dstinv:subscribed{object = user} then return end
	-- Validate slots.
	if type(srcslot) == "string" then
		if not srcinv.owner.spec.equipment_slots then return end
		if not srcinv.owner.spec.equipment_slots[srcslot] then return end
	elseif srcslot < 1 or srcinv.size < srcslot then
		return
	end
	if type(dstslot) == "string" then
		if not dstinv.owner.spec.equipment_slots then return end
		if not dstinv.owner.spec.equipment_slots[dstslot] then return end
	elseif dstslot < 1 then
		dstslot = dstinv:get_empty_slot()
		if not dstslot then return end
	elseif dstslot > dstinv.size then
		return
	end
	-- Try to move the item.
	local srcobj = srcinv:get_object{slot = srcslot}
	if not srcobj then return end
	local dstobj = dstinv:get_object{slot = dstslot}
	if not dstobj then
		srcobj:detach()
		dstinv:set_object{slot = dstslot, object = srcobj}
		return true
	end
	-- Try to merge with other items.
	if dstinv:merge_object{object = srcobj, slot = dstslot} then
		return true
	end
	-- Try to displace the other item.
	local tmp = inv:get_empty_slot()
	if not tmp then return end
	dstobj:detach()
	dstinv:set_object{slot = tmp, object = dstobj}
	srcobj:detach()
	dstinv:set_object{slot = slot, object = srcobj}
	return true
end

--- Drops an inventory item.
-- @param clss Actions class.
-- @param user Object performing the action.
-- @param srcid Source inventory ID.
-- @param srcslot Source inventory or equipment slot.
-- @return True on success.
Actions.move_from_inv_to_world = function(clss, user, srcid, srcslot)
	-- Find the inventory.
	local srcinv = Inventory:find{id = srcid}
	if not srcinv or not srcinv:subscribed{object = user} then return end
	-- Move the item.
	local srcobj = srcinv:get_object{slot = srcslot}
	if not srcobj then return end
	srcobj:detach()
	srcobj.position = user.position
	srcobj.velocity = Vector()
	srcobj.rotation = Quaternion()
	srcobj.realized = true
	-- Animate the user.
	user:animate{animation = "pickup", channel = Animation.CHANNEL_ACTION, weight = 10}
	user.cooldown = 1
	return true
end

--- Picks up an item and places it into the inventory.
-- @param clss Actions class.
-- @param user Object performing the action.
-- @param srcid Source object ID.
-- @param dstid Destination inventory ID.
-- @param dstslot Destination inventory or equipment slot.
-- @return True on success.
Actions.move_from_world_to_inv = function(clss, user, srcid, dstid, dstslot)
	-- Find the inventory.
	local dstinv = Inventory:find{id = dstid}
	if not dstinv or not dstinv:subscribed{object = user} then return end
	-- Validate slots.
	if type(dstslot) == "string" then
		if not dstinv.owner.spec.equipment_slots then return end
		if not dstinv.owner.spec.equipment_slots[dstslot] then return end
	elseif dstslot < 1 then
		dstslot = nil
	elseif dstslot > dstinv.size then
		return
	end
	-- Find the world item.
	local srcobj = Object:find{id = srcid, point = user.position, radius = 5.0}
	if not srcobj or not srcobj.pickable then return end
	-- Try to move the item.
	local dstobj = dstinv:get_object{slot = dstslot}
	if not dstobj and dstslot then
		srcobj:detach()
		dstinv:set_object{slot = dstslot, object = srcobj}
	-- Try to merge with other items.
	elseif not dstinv:merge_object{object = srcobj, slot = dstslot} then
		return
	end
	-- Animate the user.
	user:animate{animation = "pickup", channel = Animation.CHANNEL_ACTION, weight = 10}
	user.cooldown = 1
	return true
end
