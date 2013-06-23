local Item = require("core/objects/item")

Actionspec{
	name = "harvest",
	label = "Harvest",
	func = function(self, user)
		-- Create the list of harvest results.
		local mats = {}
		for k,v in pairs(self.spec.harvest_materials) do table.insert(mats, k) end
		if #mats == 0 then return end
		-- Play the harvesting effect.
		Server:world_effect(self:get_position(), self.spec.harvest_effect)
		-- Choose a random item from the list.
		local item = Item(user.manager)
		item:set_spec(Itemspec:find_by_name(mats[math.random(1, #mats)]))
		user.inventory:merge_or_drop_object(item)
		user:send_message("Harvested " .. item.spec.name .. ".")
		-- Apply the harvesting behavior.
		if self.spec.harvest_behavior == "destroy" then
			self:die()
		end
	end}
