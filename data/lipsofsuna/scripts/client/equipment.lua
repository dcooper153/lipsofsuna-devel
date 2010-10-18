Equipment = Class()

Equipment.init = function(clss)
	clss.group = Group{cols = 1, style = "equipment"}
	clss.group:set_request{height = 155, width = 60}
	clss.button_head = Button{style = "label"}
	clss.button_upperbody = Button{style = "label"}
	clss.button_handl = Button{style = "label"}
	clss.button_handr = Button{style = "label"}
	clss.button_lowerbody = Button{style = "label"}
	clss.button_feet = Button{style = "label"}
	clss.group:append_row(clss.button_head)
	clss.group:append_row(clss.button_upperbody)
	clss.group:append_row(clss.button_handr)
	clss.group:append_row(clss.button_handl)
	clss.group:append_row(clss.button_lowerbody)
	clss.group:append_row(clss.button_feet)
	clss.group:set_expand{col = 1}
	clss.button_head.pressed = function() clss:clicked("head") end
	clss.button_upperbody.pressed = function() clss:clicked("upperbody") end
	clss.button_handl.pressed = function() clss:clicked("hand.L") end
	clss.button_handr.pressed = function() clss:clicked("hand.R") end
	clss.button_lowerbody.pressed = function() clss:clicked("lowerbody") end
	clss.button_feet.pressed = function() clss:clicked("feet") end
end

--- Sets the contents of an equipment slot.
-- @param clss Equipment class.
-- @param node Node name.
-- @param name Item name.
Equipment.set_item = function(clss, slot, name)
	local funs =
	{
		["head"] = function() clss.button_head.text = name end,
		["upperbody"] = function() clss.button_upperbody.text = name end,
		["hand.L"] = function() clss.button_handl.text = name end,
		["hand.R"] = function() clss.button_handr.text = name end,
		["lowerbody"] = function() clss.button_lowerbody.text = name end,
		["feet"] = function() clss.button_feet.text = name end
	}
	local fun = funs[slot]
	if fun then fun() end
end

Equipment.clicked = function(clss, slot)
	if Target:active() then
		Target:select_equipment(slot)
	else
		Target:start("Where would you like to move that?", function(where, id, slott)
			Equipment:move("equ", nil, slot, where, id, slott) end)
	end
end

Equipment.move = function(self, src_type, src_id, src_slot, dst_type, dst_id, dst_slot)

	if not Player.object then return end
	local pos = Player.object.position

	-- Equipment -> equipment.
	if src_type == "equ" and dst_type == "equ" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.EQUIPMENT, "uint8", moveitem.EQUIPMENT,
			"string", src_slot,
			"string", dst_slot)}

	-- Equipment -> inventory.
	elseif src_type == "equ" and dst_type == "inv" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.EQUIPMENT, "uint8", moveitem.INVENTORY,
			"string", src_slot,
			"uint32", dst_id, "uint32", dst_slot)}

	-- Equipment -> world.
	elseif src_type == "equ" and dst_type == "obj" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.EQUIPMENT, "uint8", moveitem.WORLD,
			"string", src_slot,
			"float", pos.x, "float", pos.y, "float", pos.z, "float", 0.0)}

	-- Inventory -> equipped.
	elseif src_type == "inv" and dst_type == "equ" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.INVENTORY, "uint8", moveitem.EQUIPMENT,
			"uint32", src_id, "uint32", src_slot,
			"string", dst_slot)}

	-- Inventory -> inventory.
	elseif src_type == "inv" and dst_type == "inv" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.INVENTORY, "uint8", moveitem.INVENTORY,
			"uint32", src_id, "uint32", src_slot,
			"uint32", dst_id, "uint32", dst_slot)}

	-- Inventory -> world.
	elseif src_type == "inv" and dst_type == "obj" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.INVENTORY, "uint8", moveitem.WORLD,
			"uint32", src_id, "uint32", src_slot,
			"float", pos.x, "float", pos.y, "float", pos.z, "float", 0.0)}

	-- World -> equipment.
	elseif src_type == "obj" and dst_type == "equ" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.WORLD, "uint8", moveitem.EQUIPMENT,
			"uint32", src_id,
			"string", dst_slot)}

	-- World -> inventory.
	elseif src_type == "obj" and dst_type == "inv" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.WORLD, "uint8", moveitem.EQUIPMENT,
			"uint32", src_id,
			"uint32", dst_id, "uint32", dst_slot)}

	-- World -> world.
	elseif src_type == "obj" and dst_type == "obj" then
		Network:send{packet = Packet(packets.MOVE_ITEM,
			"uint8", moveitem.WORLD, "uint8", moveitem.WORLD,
			"uint32", src_id,
			"float", pos.x, "float", pos.y, "float", pos.z, "float", 0.0)}
	end

end

------------------------------------------------------------------------------

Equipment:init()

-- Updates items of the equipment display.
Protocol:add_handler{type = "OBJECT_SLOT", func = function(event)
	local ok,i,slot,node,model,name = event.packet:read("uint32", "string", "string", "string", "string")
	if ok and Player.object and Player.object.id == i then
		Equipment:set_item(slot, name)
	end
end}
