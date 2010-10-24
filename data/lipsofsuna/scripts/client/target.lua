Target = Class()
Target.func = nil
Target.target_object = nil

--- Checks if waiting for target.
-- @param self Target class.
-- @return Boolean.
function Target.active(self)
	return self.action.enabled
end

--- Cancels targeting.
-- @param self Target class.
function Target.cancel(self)
	Gui:set_action_text()
	self.func = nil
	self.action.enabled = false
	controls.EDIT_SELECT.enabled = not Client.moving
	controls.SHOOT.enabled = Client.moving
end

--- Picks an object or a tile from the scene based on the look vector of the player.
-- @param clss Target class.
-- @param args Arguments or nil.<ul>
--   <li>ignore: Dictionary of objects to ignore or nil.</li></ul>
-- @return Vector and object
Target.pick_look = function(clss, args)
	if not Player.object then return end
	local r1,r2 = Player:get_picking_ray_1st()
	return clss:pick_ray{ray1 = r1, ray2 = r2, ignore = args and args.ignore}
end

--- Picks an object or a tile from the scene based on a ray.
-- @param clss Target class.
-- @param args Arguments.<ul>
--   <li>ignore: Dictionary of objects to ignore or nil.</li>
--   <li>ray1: Start point of the ray.</li>
--   <li>ray2: End point of the ray.</li></ul>
-- @return Vector and object
Target.pick_ray = function(clss, args)
	local best_object = nil
	local best_point = nil
	local best_dist = nil
	local r1 = args.ray1
	local r2 = args.ray2
	local ignore = args.ignore or {}
	-- Intersect with objects.
	for id,o in pairs(Object.objects) do
		if o.realized and not ignore[o] then
			local p = o:intersect_ray(r1, r2)
			if p then
				local d = (r1 - p).length
				if not best_dist or d < best_dist then
					best_dist = d
					best_point = p
					best_object = o
				end
			end
		end
	end
	-- Intersect with terrain.
	local p = Voxel:intersect_ray(r1, best_point or r2)
	if p then
		best_point = p
		best_object = nil
	end
	return best_point,best_object
end

--- Picks an object or a tile from the scene based on camera orientation.
-- @param clss Target class.
-- @return Vector and object
Target.pick_scene = function(clss)
	local r1,r2 = Player.camera:picking_ray{cursor = Client.cursor_pos}
	return clss:pick_ray{ray1 = r1, ray2 = r2}
end

--- Initiates targeting.
-- @param clss Target class.
-- @param func Targeting callback.
Target.start = function(clss, msg, func)
	if Client.moving then
		if clss.target_object then
			func("obj", clss.target_object.id)
		end
	else
		Gui:set_action_text(msg)
		clss.func = func
		clss.action.enabled = true
		controls.EDIT_SELECT.enabled = true
		controls.SHOOT.enabled = false
	end
end

--- Finishes targeting by selecting an item from a container.
-- @param self Target class.
-- @param inv Container number.
-- @param slot Slot number.
function Target.select_container(self, inv, slot)
	if self.action.enabled then
		local func = self.func
		self:cancel()
		func("inv", inv, slot)
	end
end

--- Finishes targeting by selecting an equipment slot.
-- @param self Target class.
-- @param slot Slot name.
function Target.select_equipment(self, slot)
	if self.action.enabled then
		local func = self.func
		self:cancel()
		func("equ", 0, slot)
	end
end

--- Finishes targeting by selecting an object from under the cursor.
-- @param self Target class.
function Target.select_scene(self)
	if self.action.enabled then
		local func = self.func
		self:cancel()
		local pos,obj = Target:pick_scene()
		if obj then
			func("obj", obj.id)
		else
			func("map", nil, pos)
		end
	end
end

Target.action = Action{id = "target", name = "Target", desc = "Target an object or creature under the cursor"}
Target.action.enabled = false
Target.action.callback = function(event)
	if event.active then Target:select_scene() end
end
Binding{action = "target", mousebutton = 1} --BUTTON1

-- Periodically check if the're an object in front of the player.
Timer{delay = 0.2, func = function()
	if Player.object and Client.moving then
		-- Ignore the player and the equipment.
		local ignore = {[Player.object] = true}
		local slots = Slots:find{owner = Player.object}
		if slots then
			for k,v in pairs(slots.slots) do
				ignore[v] = true
			end
		end
		-- Ray pick an object in front of the player.
		local p,o = Target:pick_look{ignore = ignore}
		if o then
			Gui.label_target.text = "Use " .. (o.name or "")
		else
			Gui.label_target.text = ""
		end
		Target.target_object = o
	else
		Target.target_object = nil
	end
end}
