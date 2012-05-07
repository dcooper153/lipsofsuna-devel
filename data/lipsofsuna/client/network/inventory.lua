Protocol:add_handler{type = "INVENTORY_CLOSED", func = function(event)
	-- Parse the packet.
	local ok,id = event.packet:read("uint32")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	-- Clear the inventory.
	object.inventory:clear()
	object.inventory.size = 0
end}

Protocol:add_handler{type = "INVENTORY_CREATED", func = function(event)
	-- Parse the packet.
	local ok,id,type,name,size,own = event.packet:read("uint32", "string", "string", "uint8", "bool")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	-- Initialize the inventory.
	object.inventory:clear()
	object.inventory.size = size
	-- Show the inventory.
	if not own then
		Client.data.inventory.id = id
		if Ui.state == "inventory" then
			Ui:push_state("loot")
		else
			Ui.state = "loot"
		end
	end
end}

Protocol:add_handler{type = "INVENTORY_ITEM_ADDED", func = function(event)
	-- Parse the packet.
	local ok,id,index,name,count = event.packet:read("uint32", "uint8", "string", "uint32")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	-- Find the itemspec.
	local spec = Itemspec:find{name = name}
	if not spec then return end
	-- Add to the inventory.
	object.inventory:set_object(index, Item{spec = spec, count = count})
	-- Update the user interface.
	if Ui.state == "inventory" and object == Client.player_object then
		Ui:restart_state()
	elseif Ui.state == "loot" and object.id == Client.data.inventory.id then
		Ui:restart_state()
	elseif Ui.state == "store" and object == Client.player_object then
		Ui:restart_state()
	end
end}

Protocol:add_handler{type = "INVENTORY_ITEM_EQUIPPED", func = function(event)
	-- Parse the packet.
	local ok,id,index,slot = event.packet:read("uint32", "uint8", "string")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	-- Equip the index.
	object.inventory:equip_index(index, slot)
	-- Update the user interface.
	if Ui.state == "inventory" and object == Client.player_object then
		Ui:restart_state()
	elseif Ui.state == "loot" and object.id == Client.data.inventory.id then
		Ui:restart_state()
	elseif Ui.state == "store" and object == Client.player_object then
		Ui:restart_state()
	end
end}

Protocol:add_handler{type = "INVENTORY_ITEM_REMOVED", func = function(event)
	-- Parse the packet.
	local ok,id,index = event.packet:read("uint32", "uint8")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	-- Remove from the inventory.
	object.inventory:set_object(index)
	-- Update the user interface.
	if Ui.state == "inventory" and object == Client.player_object then
		Ui:restart_state()
	elseif Ui.state == "loot" and object.id == Client.data.inventory.id then
		Ui:restart_state()
	elseif Ui.state == "store" and object == Client.player_object then
		Ui:restart_state()
	end
end}

Protocol:add_handler{type = "INVENTORY_ITEM_UNEQUIPPED", func = function(event)
	-- Parse the packet.
	local ok,id,index = event.packet:read("uint32", "uint8")
	if not ok then return end
	-- Find the object.
	local object = Object:find{id = id}
	if not object then return end
	-- Unequip the index.
	object.inventory:unequip_index(index)
	-- Update the user interface.
	if Ui.state == "inventory" and object == Client.player_object then
		Ui:restart_state()
	elseif Ui.state == "loot" and object.id == Client.data.inventory.id then
		Ui:restart_state()
	elseif Ui.state == "store" and object == Client.player_object then
		Ui:restart_state()
	end
end}
