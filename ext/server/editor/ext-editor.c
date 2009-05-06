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
 * \addtogroup liextsrvEditor Editor      
 * @{
 */

#include <network/lips-network.h>
#include <server/lips-server.h>
#include "ext-editor.h"

static int
private_client_packet (liextEditor* self,
	                   lisrvClient* client,
	                   liReader*    packet);

/*****************************************************************************/

liextEditor*
liext_editor_new (lisrvServer* server)
{
	liextEditor* self;

	self = calloc (1, sizeof (liextEditor));
	if (self == NULL)
		return NULL;
	self->server = server;
	self->calls[0] = lieng_engine_call_insert (server->engine, LISRV_CALLBACK_CLIENT_PACKET, 1, private_client_packet, self);
	if (self->calls[0] == NULL)
	{
		free (self);
		return NULL;
	}

	return self;
}

void
liext_editor_free (liextEditor* self)
{
	lieng_engine_call_remove (self->server->engine, LISRV_CALLBACK_CLIENT_PACKET, self->calls[0]);
	free (self);
}

/*****************************************************************************/

static int
private_client_packet (liextEditor* self,
	                   lisrvClient* client,
	                   liReader*    packet)
{
	uint8_t cmd;
	uint32_t id;
	liengModel* model;
	liengObject* object;
	limatQuaternion rotation;
	limatTransform transform;
	limatVector position;

	if (((uint8_t*) packet->buffer)[0] != LINET_EXT_CLIENT_PACKET_EDITOR)
		return 1;
	if (!li_reader_get_uint8 (packet, &cmd))
		return 1;
	switch (cmd)
	{
		case LINET_EXT_EDITOR_PACKET_CREATE:
			if (!li_reader_get_uint32 (packet, &id) ||
				!li_reader_get_float (packet, &position.x) ||
				!li_reader_get_float (packet, &position.y) ||
				!li_reader_get_float (packet, &position.z) ||
				!li_reader_get_float (packet, &rotation.x) ||
				!li_reader_get_float (packet, &rotation.y) ||
				!li_reader_get_float (packet, &rotation.z) ||
				!li_reader_get_float (packet, &rotation.w))
				return 1;
			model = lieng_engine_find_model_by_code (self->server->engine, id);
			if (model == NULL)
				return 1;
			object = lieng_object_new (self->server->engine, model,
				LIPHY_SHAPE_MODE_CONCAVE, LIPHY_CONTROL_MODE_STATIC, 0, NULL);
			if (object == NULL)
				return 1;
			transform = limat_transform_init (position, rotation);
			lieng_object_set_transform (object, &transform);
			lieng_object_set_realized (object, 1);
			break;
		case LINET_EXT_EDITOR_PACKET_DESTROY:
			if (!li_reader_get_uint32 (packet, &id))
				return 1;
			object = lieng_engine_find_object (self->server->engine, id);
			if (object == NULL)
				return 1;
			lieng_object_set_realized (object, 0);
			break;
		case LINET_EXT_EDITOR_PACKET_SAVE:
			lieng_engine_save (self->server->engine);
			break;
		case LINET_EXT_EDITOR_PACKET_TRANSFORM:
			if (!li_reader_get_uint32 (packet, &id) ||
				!li_reader_get_float (packet, &position.x) ||
				!li_reader_get_float (packet, &position.y) ||
				!li_reader_get_float (packet, &position.z) ||
				!li_reader_get_float (packet, &rotation.x) ||
				!li_reader_get_float (packet, &rotation.y) ||
				!li_reader_get_float (packet, &rotation.z) ||
				!li_reader_get_float (packet, &rotation.w))
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
