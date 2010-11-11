/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtNetwork Network
 * @{
 */

#include "ext-module.h"
#include "ext-client.h"

/* @luadoc
 * module "core/network"
 * --- Networking support.
 * -- @name Network
 * -- @class table
 */

/* @luadoc
 * --- Disconnects a client.
 * --
 * -- @param clss Network class.
 * -- @param args Arguments.<ul>
 * --   <li>1,client: Client number. (required)</li></ul>
 * function Network.disconnect(clss)
 */
static void Network_disconnect (LIScrArgs* args)
{
	int id;
	LIExtClient* client;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	if (!liscr_args_geti_int (args, 0, &id) &&
	    !liscr_args_gets_int (args, "client", &id))
		return;
	client = liext_network_find_client (module, id);
	if (client != NULL)
		liext_client_free (client);
}

/* @luadoc
 * --- Begins listening for clients.
 * --
 * -- @param clss Network class.
 * -- @param args Arguments.<ul>
 * --   <li>port: Port to listen to.</li>
 * --   <li>udp: True for UDP.</li></ul>
 * -- @return True on success.
 * function Network.host(clss, args)
 */
static void Network_host (LIScrArgs* args)
{
	int port = 10101;
	int udp = 0;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	liscr_args_gets_bool (args, "udp", &udp);
	liscr_args_gets_int (args, "port", &port);
	port = LIMAT_CLAMP (port, 1025, 32767);

	if (liext_network_host (module, port))
		liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * --- Connects to a server.
 * --
 * -- @param clss Network class.
 * -- @param args Arguments.<ul>
 * --   <li>host: Server address.</li>
 * --   <li>port: Port to listen to.</li>
 * -- @return True on success.
 * function Network.join(clss, args)
 */
static void Network_join (LIScrArgs* args)
{
	int port = 10101;
	const char* addr = "localhost";
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	liscr_args_gets_int (args, "port", &port);
	liscr_args_gets_string (args, "host", &addr);
	port = LIMAT_CLAMP (port, 1025, 32767);

	if (liext_network_join (module, port, addr))
		liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * --- Sends a network packet to the client controlling the object.
 * --
 * -- @param clss Network class.
 * -- @param args Arguments.<ul>
 * --   <li>client: Client ID. (required if hosting)</li>
 * --   <li>packet: Packet. (required)</li>
 * --   <li>reliable: Boolean.</li></ul>
 * function Network.send(clss, args)
 */
static void Network_send (LIScrArgs* args)
{
	int id = 0;
	int reliable = 1;
	LIExtModule* module;
	LIScrData* data;
	LIScrPacket* packet;

	/* Get packet. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);

	/* Get object if hosting. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	if (module->server_socket)
	{
		if (!liscr_args_gets_int (args, "client", &id))
			return;
	}

	/* Send packet. */
	liscr_args_gets_bool (args, "reliable", &reliable);
	if (packet->writer != NULL)
		liext_network_send (module, id, packet->writer, reliable);
}

/* @luadoc
 * --- Disconnects all client and closes the network connection.
 * --
 * -- @param clss Network class.
 * function Network.shutdown(clss)
 */
static void Network_shutdown (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	liext_network_shutdown (module);
}

/* @luadoc
 * --- Gets the list of connected clients.
 * --
 * -- @name Network.clients
 * -- @class table
 */
static void Network_getter_clients (LIScrArgs* args)
{
	LIAlgU32dicIter iter;
	LIExtClient* client;
	LIExtModule* module;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	LIALG_U32DIC_FOREACH (iter, module->clients)
	{
		client = iter.value;
		if (client->connected)
			liscr_args_seti_int (args, client->id);
	}
}

/* @luadoc
 * --- Controls whether clients can connect to the server.
 * --
 * -- @name Network.closed
 * -- @class table
 */
static void Network_getter_closed (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	liscr_args_seti_bool (args, liext_network_get_closed (module));
}
static void Network_setter_closed (LIScrArgs* args)
{
	int value;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	if (liscr_args_geti_bool (args, 0, &value))
		liext_network_set_closed (module, value);
}

/* @luadoc
 * --- Gets whether the game connected to network.
 * --
 * -- @name Network.connected
 * -- @class table
 */
static void Network_getter_connected (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	liscr_args_seti_bool (args, liext_network_get_connected (module));
}

void
liext_script_network (LIScrClass* self,
                      void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_NETWORK, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "disconnect", Network_disconnect);
	liscr_class_insert_cfunc (self, "host", Network_host);
	liscr_class_insert_cfunc (self, "join", Network_join);
	liscr_class_insert_cfunc (self, "send", Network_send);
	liscr_class_insert_cfunc (self, "shutdown", Network_shutdown);
	liscr_class_insert_cvar (self, "clients", Network_getter_clients, NULL);
	liscr_class_insert_cvar (self, "closed", Network_getter_closed, Network_setter_closed);
	liscr_class_insert_cvar (self, "connected", Network_getter_connected, NULL);
}

/** @} */
/** @} */
