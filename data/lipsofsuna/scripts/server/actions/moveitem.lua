--- Moves an item.
-- @param clss Actions class.
-- @param args Arguments.<ul>
--   <li>dstinv: Destination inventory ID.</li>
--   <li>dstpoint: Destination position in world coordinates.</li>
--   <li>dstslot: Destination equipment slot name or inventory slot number.</li>
--   <li>object: Object performing the action.</li>
--   <li>rotation: Rotation when moved to the world.</li>
--   <li>srcobj: Source map object ID.</li>
--   <li>srcinv: Source inventory ID.</li>
--   <li>srcslot: Source equipment slot name.</li>
--   <li>velocity: Velocity when moved to the world.</li></ul>
-- @return True on success.
Actions:register{name = "moveitem", func = function(clss, args)
	local object = args.object
	-- Get moved object.
	local srcobj = nil
	if args.srcobj then
		-- TODO: Check for walls.
		srcobj = Object:find{id = args.srcobj, point = object.position, radius = 5.0}
		if not srcobj or not srcobj.pickable then return end
	elseif args.srcinv then
		local inv = Inventory:find{id = args.srcinv}
		if not inv or not inv:subscribed{object = object} then return end
		srcobj = inv:get_object{slot = args.srcslot}
		if not srcobj then return end
	elseif args.srcslot then
		local inventory = Inventory:find{owner = object}
		if not inventory then return end
		srcobj = inventory:get_object{slot = args.srcslot}
		if not srcobj then return end
	end

	-- Move to inventory.
	if args.dstinv then
		local inv = Inventory:find{id = args.dstinv}
		if not inv or not inv:subscribed{object = object} then return end
		local slot = args.dstslot
		if slot then
			if slot < 1 or slot > inv.size then return end
			local object = inv:get_object{slot = slot}
			if object then
				if object == srcobj then return end
				if not inv:merge_object{object = srcobj, slot = slot} then
					local tmp = inv:get_empty_slot()
					if not tmp then return end
					object:detach()
					inv:set_object{slot = tmp, object = object}
					srcobj:detach()
					inv:set_object{slot = slot, object = srcobj}
				end
			else
				srcobj:detach()
				inv:set_object{slot = slot, object = srcobj}
			end
		elseif not inv:merge_object{object = srcobj} then
			return
		end
	-- Move to equipment.
	elseif args.dstslot then
		if not srcobj.itemspec then return end
		if srcobj.itemspec.equipment_slot ~= args.dstslot then return end
		local inventory = Inventory:find{owner = object}
		if not inventory then return end
		local oldequ = inventory:get_object{slot = args.dstslot}
		if oldequ then
			if not inventory:merge_object{object = oldequ} then return end
		end
		srcobj:detach()
		inventory:set_object{slot = args.dstslot, object = srcobj}
	-- Move to world.
	elseif args.dstpoint then
		if (args.dstpoint - object.position).length > 5.0 then return end
		-- TODO: Check for walls.
		srcobj:detach()
		srcobj.position = args.dstpoint
		srcobj.velocity = args.velocity or Vector()
		srcobj.rotation = args.rotation or Quaternion()
		srcobj.realized = true
	else return end
	-- Animate when moving to or from the world.
	if args.srcobj or args.dstpoint then
		object:animate{animation = "pickup", channel = Animation.CHANNEL_ACTION, weight = 10}
		object.cooldown = 2
	end
	return true
end}
