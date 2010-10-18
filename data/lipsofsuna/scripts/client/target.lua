Target = {}
Target.func = nil

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

function Target.pick_scene(self)
	local best_object = nil
	local best_point = nil
	local best_dist = nil
	-- Get picking ray.
	local r1,r2 = Player.camera:picking_ray{cursor = Client.cursor_pos}
	-- Intersect with objects.
	for id,o in pairs(Object.objects) do
		if o.realized then
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

--- Initiates targeting.
-- @param self Target class.
-- @param func Targeting callback.
function Target.start(self, msg, func)
	Gui:set_action_text(msg)
	self.func = func
	self.action.enabled = true
	Client.moving = false
	controls.EDIT_SELECT.enabled = false
	controls.SHOOT.enabled = false
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
