Commands = {}

--- Initiates targeting for the examine command.
-- @param self Commands class.
Commands.examine = function(self)
	Target:start("What would you like to examine?", function(where, id, slot)
		if where == "inv" then
			Network:send{packet = Packet(packets.EXAMINE, "uint32", id, "uint32", slot)}
		elseif where == "obj" then
			Network:send{packet = Packet(packets.EXAMINE, "uint32", 0, "uint32", id)}
		end
	end)
end

--- Initiates targeting for the pick up command.
-- @param self Commands class.
Commands.pickup = function(self)
	Target:start("What would you like to pick up?", function(where, id, slot)
		if where == "obj" then
			Network:send{packet = Packet(packets.MOVE_ITEM,
				"uint8", moveitem.WORLD, "uint8", moveitem.INVENTORY,
				"uint32", id,
				"uint32", Inventory.id, "uint32", 0)}
		end
	end)
end

--- Initiates targeting for the throw command.
-- @param self Commands class.
Commands.throw = function(self)
	Target:start("What would you like to throw?", function(where, id, slot)
		if where == "inv" then
			Network:send{packet = Packet(packets.THROW, "uint32", id, "uint32", slot)}
		end
	end)
end

--- Initiates targeting for the use command.
-- @param self Commands class.
Commands.use = function(self)
	Target:start("What would you like to use?", function(where, id, slot)
		if where == "inv" then
			Network:send{packet = Packet(packets.USE, "uint32", id, "uint32", slot)}
		elseif where == "obj" then
			Network:send{packet = Packet(packets.USE, "uint32", 0, "uint32", id)}
		end
	end)
end
