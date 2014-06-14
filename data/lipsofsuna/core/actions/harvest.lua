local ActionSpec = require("core/specs/action")
local Item = require("core/objects/item")

ActionSpec{
	name = "harvest",
	label = "Harvest",
	start = function(action, object)
		-- Create the list of harvest results.
		local mats = {}
		for k,v in pairs(object.spec.harvest_materials) do table.insert(mats, k) end
		if #mats == 0 then return end
		-- Play the harvesting effect.
		Main.vision:world_effect(object:get_position(), object.spec.harvest_effect)
		-- Choose a random item from the list.
		local item = Item(action.object.manager)
		item:set_spec(Main.specs:find_by_name("ItemSpec", mats[math.random(1, #mats)]))
		action.object.inventory:merge_or_drop_object(item)
		action.object:send_message("Harvested " .. item.spec.name .. ".")
		-- Apply the harvesting behavior.
		if object.spec.harvest_behavior == "destroy" then
			object:die()
		end
	end}
