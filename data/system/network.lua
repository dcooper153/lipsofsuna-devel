local Class = require("system/class")

if not Los.program_load_extension("network") then
	error("loading extension `network' failed")
end

------------------------------------------------------------------------------

local Network = Class("Network")

--- Disconnects a client.
-- @param clss Network class.
-- @param args Arguments.<ul>
--   <li>1,client: Client number. (required)</li></ul>
Network.disconnect = function(clss, args)
	Los.network_disconnect(args)
end

--- Gets the address of a client.
-- @param clss Network class.
-- @param client Client number.
-- @return Client address, or nil.
Network.get_client_address = function(clss, client)
	return Los.network_get_client_address(client)
end

--- Begins listening for clients.
-- @param clss Network class.
-- @param args Arguments.<ul>
--   <li>port: Port to listen to.</li>
--   <li>udp: True for UDP.</li></ul>
-- @return True on success.
Network.host = function(clss, args)
	return Los.network_host(args)
end

--- Connects to a server.
-- @param clss Network class.
-- @param args Arguments.<ul>
--   <li>host: Server address.</li>
--   <li>port: Port to listen to.</li>
-- @return True on success.
Network.join = function(clss, args)
	return Los.network_join(args)
end

--- Sends a network packet to the client controlling the object.
-- @param clss Network class.
-- @param args Arguments.<ul>
--   <li>client: Client ID if hosting.</li>
--   <li>packet: Packet.</li>
--   <li>reliable: Boolean.</li></ul>
Network.send = function(clss, args)
	Los.network_send{client = args.client, packet = args.packet.handle, reliable = args.reliable}
end

--- Disconnects all client and closes the network connection.
-- @param clss Network class.
Network.shutdown = function(self)
	Los.network_shutdown()
end

--- Updates the network status and generates network events.
-- @param clss Network class.
Network.update = function(clss)
	Los.network_update()
end

--- Gets the list of connected clients.
-- @param clss Network class
-- @return List of client IDs.
Network.get_clients = function(self)
	return Los.network_get_clients()
end

--- Returns true if all future connections will be blocked.
-- @param clss Network class
-- @return Boolean.
Network.get_closed = function(self)
	return Los.network_get_closed()
end

--- Toggles connections blocking.
-- @param clss Network class
-- @param v True to block connections, false to accept connections.
Network.set_closed = function(self, v)
	Los.network_set_closed(v)
end

--- Returns true if a connection has been established.
-- @param clss Network class
-- @return Boolean.
Network.get_connected = function(self)
	return Los.network_get_connected()
end

return Network
