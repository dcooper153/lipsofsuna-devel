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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvEditor Editor      
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/server.h>
#include "ext-editor.h"

static int
private_client_packet (LIExtEditor* self,
                       LISerClient* client,
                       LIArcReader* packet);

/*****************************************************************************/

LIExtEditor*
liext_editor_new (LISerServer* server)
{
	LIExtEditor* self;

	self = lisys_calloc (1, sizeof (LIExtEditor));
	if (self == NULL)
		return NULL;
	self->server = server;
	if (!lical_callbacks_insert (server->callbacks, server->engine, "client-packet", 1, private_client_packet, self, self->calls + 0))
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
liext_editor_free (LIExtEditor* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int
private_client_packet (LIExtEditor* self,
                       LISerClient* client,
                       LIArcReader* packet)
{
	uint8_t cmd;
	uint32_t id;
	LIEngModel* model;
	LIEngObject* object;
	LIMatQuaternion rotation;
	LIMatTransform transform;
	LIMatVector position;

	if (((uint8_t*) packet->buffer)[0] != LINET_EXT_CLIENT_PACKET_EDITOR)
		return 1;
	if (!liarc_reader_get_uint8 (packet, &cmd))
		return 1;
	switch (cmd)
	{
		case LINET_EXT_EDITOR_PACKET_CREATE:
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
			object = lieng_object_new (self->server->engine, model, LIPHY_CONTROL_MODE_STATIC,
				liser_server_get_unique_object (self->server));
			if (object == NULL)
				return 1;
			lieng_object_set_collision_group (object, LIPHY_GROUP_STATICS);
			lieng_object_set_collision_mask (object, ~(LIPHY_GROUP_STATICS | LIPHY_GROUP_TILES));
			transform = limat_transform_init (position, rotation);
			lieng_object_set_transform (object, &transform);
			lieng_object_set_realized (object, 1);
			break;
		case LINET_EXT_EDITOR_PACKET_DESTROY:
			if (!liarc_reader_get_uint32 (packet, &id))
				return 1;
			object = lieng_engine_find_object (self->server->engine, id);
			if (object == NULL)
				return 1;
			lieng_object_set_realized (object, 0);
			liser_object_purge (object);
			break;
		case LINET_EXT_EDITOR_PACKET_SAVE:
			liser_server_save (self->server);
			break;
		case LINET_EXT_EDITOR_PACKET_TRANSFORM:
			if (!liarc_reader_get_uint32 (packet, &id) ||
				!liarc_reader_get_float (packet, &position.x) ||
				!liarc_reader_get_float (packet, &position.y) ||
				!liarc_reader_get_float (packet, &position.z) ||
				!liarc_reader_get_float (packet, &rotation.x) ||
				!liarc_reader_get_float (packet, &rotation.y) ||
				!liarc_reader_get_float (packet, &rotation.z) ||
				!liarc_reader_get_float (packet, &rotation.w))
				return 1;
			object = lieng_engine_find_object (self->server->engine, id);
			if (object == NULL)
				return 1;
			transform = limat_transform_init (position, rotation);
			lieng_object_set_transform (object, &transform);
			break;
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
