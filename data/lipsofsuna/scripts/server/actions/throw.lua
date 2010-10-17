--- Throw an inventory item.
-- @param self Object.
-- @param inv Inventory number.
-- @param slot Inventory slot number.
-- @return True if throwing succeeded.
--[[
Actions:register{name = "throw", func = function(self, inv, slot)

	-- Find inventory.
	local inventory = Inventory:find{id = inv}
	if not inventory or not inventory:subscribed{object = self} then
		return false
	end

	-- Find object.
	local object = inventory:get_object{slot = slot}
	if not object then
		return false
	end

	-- Throw object.
	inventory:set_object{slot = slot}
	local dir = self.rotation * Vector(0, 0, -1)
	object.position = self.position + dir + Vector(0, 1, 0)
	object.rotation = self.rotation
	object.velocity = dir * 30
	object.realized = true
	self:effect{effect = "wield-000"}
	return true
end}
--]]
