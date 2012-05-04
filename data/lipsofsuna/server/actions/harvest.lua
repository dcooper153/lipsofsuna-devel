Actionspec:extend{
	name = "harvest",
	func = function(self, user)
		-- Create the list of harvest results.
		local mats = {}
		for k,v in pairs(self.spec.harvest_materials) do table.insert(mats, k) end
		if #mats == 0 then return end
		-- Play the harvesting effect.
		if self.spec.harvest_effect then
			Effect:play{effect = self.spec.harvest_effect, point = self.position}
		end
		-- Choose a random item from the list.
		local item = Item{spec = Itemspec:find{name = mats[math.random(1, #mats)]}}
		user.inventory:merge_or_drop_object(item)
		user:send{packet = Packet(packets.MESSAGE, "string", "Harvested " .. item.name .. ".")}
		-- Apply the harvesting behavior.
		if self.spec.harvest_behavior == "destroy" then
			self:die()
		end
	end}
