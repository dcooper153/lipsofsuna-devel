/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvGenerator Generator
 * @{
 */

#include <network/lips-network.h>
#include <server/lips-server.h>
#include "ext-generator.h"

static int
private_client_packet (liextGenerator* self,
                       lisrvClient*    client,
                       liarcReader*    packet);

/*****************************************************************************/

liextGenerator*
liext_generator_new (lisrvServer* server)
{
	liextGenerator* self;

	self = lisys_calloc (1, sizeof (liextGenerator));
	if (self == NULL)
		return NULL;
	self->server = server;
	if (!lieng_engine_insert_call (server->engine, LISRV_CALLBACK_CLIENT_PACKET, 1,
	     	private_client_packet, self, self->calls + 0))
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
liext_generator_free (liextGenerator* self)
{
	lieng_engine_remove_calls (self->server->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int
private_client_packet (liextGenerator* self,
	                   lisrvClient*    client,
	                   liarcReader*    packet)
{
	uint8_t cmd;
	uint32_t id;
	liengModel* model;
	liengObject* object;
	limatQuaternion rotation;
	limatTransform transform;
	limatVector position;

	if (((uint8_t*) packet->buffer)[0] != LINET_EXT_CLIENT_PACKET_GENERATOR)
		return 1;
	if (!liarc_reader_get_uint8 (packet, &cmd))
		return 1;
	switch (cmd)
	{
		case LINET_EXT_GENERATOR_PACKET_CREATE:
			if (!liarc_reader_get_uint32 (packet, &id) ||
				!liarc_reader_get_float (packet, &position.x) ||
				!liarc_reader_get_float (packet, &position.y) ||
				!liarc_reader_get_float (packet, &position.z) ||
				!liarc_reader_get_float (packet, &rotation.x) ||
				!liarc_reader_get_float (packet, &rotation.y) ||
				!liarc_reader_get_float (packet, &rotation.z) ||
				!liarc_reader_get_float (packet, &rotation.w))
				return 1;
			model = lieng_engine_find_model_by_code (self->server->engine, id);
			if (model == NULL)
				return 1;
			object = lieng_object_new (self->server->engine, model,
				LIPHY_SHAPE_MODE_CONCAVE, LIPHY_CONTROL_MODE_STATIC, 0, NULL);
			if (object == NULL)
				return 1;
			lieng_object_set_collision_group (object, LIPHY_GROUP_STATICS);
			lieng_object_set_collision_mask (object, ~(LIPHY_GROUP_STATICS | LIPHY_GROUP_TILES));
			transform = limat_transform_init (position, rotation);
			lieng_object_set_transform (object, &transform);
			lieng_object_set_realized (object, 1);
			break;
		case LINET_EXT_GENERATOR_PACKET_SAVE:
			lisrv_server_save (self->server);
			break;
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
