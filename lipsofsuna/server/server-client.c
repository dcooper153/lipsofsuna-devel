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
 * \addtogroup liser Server
 * @{
 * \addtogroup LISerClient Client
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/system.h>
#include "server.h"
#include "server-callbacks.h"
#include "server-client.h"
#include "server-script.h"

#define LISER_CLIENT_DEFAULT_RADIUS 24
#define LISER_CLIENT_LOAD_RADIUS 24

static void
private_callbacks_setup (LISerClient* self,
                         LIEngEngine* engine);

static void
private_callbacks_clear (LISerClient* self,
                         LIEngEngine* engine);

static int
private_object_animation (LISerClient*   self,
                          LIEngObject*   object,
                          LISerAniminfo* info);

static int
private_object_model (LISerClient* self,
                      LIEngObject* object,
                      LIEngModel*  model);

static int
private_object_motion (LISerClient* self,
                       LIEngObject* object);

static int
private_object_sample (LISerClient* self,
                       LIEngObject* object,
                       LIEngSample* sample,
                       int          flags);

static int
private_object_visibility (LISerClient* self,
                           LIEngObject* object,
                           int          visible);

static void
private_vision_clear (LISerClient*  self);

static int
private_vision_contains (LISerClient* self,
                         LIEngObject* object);

static void
private_vision_insert (LISerClient* self,
                       LIEngObject* object);

static void
private_vision_remove (LISerClient* self,
                       LIEngObject* object);

static void
private_vision_motion (LISerClient* self,
                       LIEngObject* object);

static void
private_vision_update (LISerClient* self);

/*****************************************************************************/

/**
 * \brief Allocates a new client block.
 *
 * Only called by #liser_network_update.
 *
 * \param server Server.
 * \param object Object associated to the client.
 * \param user Network user.
 * \return New client or NULL.
 */
LISerClient*
liser_client_new (LISerServer* server,
                  LIEngObject* object,
                  grapple_user user)
{
	LISerClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LISerClient));
	if (self == NULL)
		return NULL;
	self->server = server;
	self->radius = LISER_CLIENT_DEFAULT_RADIUS;
	self->network.user = user;

	/* Allocate vision. */
	self->vision = lialg_u32dic_new ();
	if (self->vision == NULL)
		goto error;

	/* Assign object. */
	if (!liser_client_set_object (self, object))
		goto error;

	return self;

error:
	if (self->vision != NULL)
		lialg_u32dic_free (self->vision);
	lisys_free (self);
	return NULL;
}

/**
 * \brief Frees the client.
 *
 * Only called by #liser_object_disconnect.
 *
 * \param self Client.
 */
void
liser_client_free (LISerClient* self)
{
	/* Remove from the client list. */
	if (self->network.user != 0)
	{
		lialg_u32dic_remove (self->server->network->clients, self->network.user);
		grapple_server_disconnect_client (self->server->network->socket, self->network.user);
	}

	/* Free vision buffer. */
	if (self->vision != NULL)
	{
		private_vision_clear (self);
		lialg_u32dic_free (self->vision);
	}

	/* Free callbacks. */
	private_callbacks_clear (self, self->server->engine);
	lisys_free (self);
}

/**
 * \brief Sets the object controlled by the client.
 *
 * Only called by #liser_object_set_client.
 *
 * \param self Client.
 * \param value Object.
 * \return Nonzero on success.
 */
int
liser_client_set_object (LISerClient* self,
                         LIEngObject* value)
{
	assert (value != NULL);

	/* Remove objects from vision. */
	private_vision_clear (self);
	private_callbacks_clear (self, self->server->engine);

	/* Set the new object. */
	self->object = value;
	private_callbacks_setup (self, self->server->engine);

	/* Insert objects to vision. */
	private_vision_update (self);

	return 1;
}

/**
 * \brief Sends a network packet to the client.
 *
 * \param self Client.
 * \param writer Packet.
 * \param flags Grapple send flags.
 */
void
liser_client_send (LISerClient* self,
                   LIArcWriter* writer,
                   int          flags)
{
	grapple_server_send (self->server->network->socket, self->network.user, flags,
		liarc_writer_get_buffer (writer),
		liarc_writer_get_length (writer));
}

int
liser_client_get_near (const LISerClient* self,
                       const LIEngObject* object)
{
	return lialg_u32dic_find (self->vision, object->id) != NULL;
}

/*****************************************************************************/

static void
private_callbacks_setup (LISerClient* self,
                         LIEngEngine* engine)
{
	lical_callbacks_insert (engine->callbacks, engine, "object-animation", 0, private_object_animation, self, self->calls + 0);
	lical_callbacks_insert (engine->callbacks, engine, "object-effect", 0, private_object_sample, self, self->calls + 1);
	lical_callbacks_insert (engine->callbacks, engine, "object-model", 0, private_object_model, self, self->calls + 2);
	lical_callbacks_insert (engine->callbacks, engine, "object-motion", 0, private_object_motion, self, self->calls + 3);
	lical_callbacks_insert (engine->callbacks, engine, "object-visibility", 0, private_object_visibility, self, self->calls + 4);
}

static void
private_callbacks_clear (LISerClient* self,
                         LIEngEngine* engine)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
}

/*****************************************************************************/

static int
private_object_animation (LISerClient*   self,
                          LIEngObject*   object,
                          LISerAniminfo* info)
{
	LIArcWriter* writer;

	/* Send to client if seen. */
	if (!private_vision_contains (self, object))
		return 1;
	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_ANIMATION);
	if (writer == NULL)
		return 1;
	liarc_writer_append_uint32 (writer, object->id);
	if (info->animation != NULL)
		liarc_writer_append_uint16 (writer, info->animation->id);
	else
		liarc_writer_append_uint16 (writer, LINET_INVALID_ANIMATION);
	liarc_writer_append_uint8 (writer, info->channel);
	liarc_writer_append_uint8 (writer, info->permanent);
	liarc_writer_append_float (writer, info->priority);
	liser_client_send (self, writer, 0);
	liarc_writer_free (writer);

	return 1;
}

static int
private_object_model (LISerClient* self,
                      LIEngObject* object,
                      LIEngModel*  model)
{
	LIArcWriter* writer;

	/* Send to client if seen. */
	if (!private_vision_contains (self, object))
		return 1;
	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_GRAPHIC);
	if (writer == NULL)
		return 1;
	liarc_writer_append_uint32 (writer, object->id);
	liarc_writer_append_uint16 (writer, lieng_object_get_model_code (object));
	liser_client_send (self, writer, GRAPPLE_RELIABLE);
	liarc_writer_free (writer);

	return 1;
}

static int
private_object_motion (LISerClient* self,
                       LIEngObject* object)
{
	float dist;
	LIMatTransform transform;

	/* Load sectors when moving. */
	if (object == self->object)
	{
		lieng_object_get_transform (self->object, &transform);
		lialg_sectors_refresh_point (self->object->engine->sectors, &transform.position, self->radius);
	}

	/* Maintain vision data. */
	dist = lieng_object_get_distance (self->object, object);
	if (dist <= self->radius)
	{
		if (object == self->object)
		{
			private_vision_update (self);
			private_vision_motion (self, object);
		}
		else if (!private_vision_contains (self, object))
			private_vision_insert (self, object);
		else
			private_vision_motion (self, object);
	}
	else
	{
		if (object == self->object)
		{
			private_callbacks_clear (self, self->server->engine);
			private_vision_clear (self);
		}
		else
		{
			if (private_vision_contains (self, object))
				private_vision_remove (self, object);
		}
	}

	return 1;
}

static int
private_object_sample (LISerClient* self,
                       LIEngObject* object,
                       LIEngSample* sample,
                       int          flags)
{
	LIArcWriter* writer;

	/* Send to client if seen. */
	if (!private_vision_contains (self, object))
		return 1;
	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_EFFECT);
	if (writer == NULL)
		return 1;
	liarc_writer_append_uint32 (writer, object->id);
	liarc_writer_append_uint16 (writer, sample->id);
	liarc_writer_append_uint16 (writer, flags);
	liser_client_send (self, writer, 0);
	liarc_writer_free (writer);

	return 1;
}

static int
private_object_visibility (LISerClient* self,
                           LIEngObject* object,
                           int          visible)
{
	float dist;

	/* Maintain vision data. */
	if (visible)
	{
		dist = lieng_object_get_distance (self->object, object);
		if (dist <= self->radius)
		{
			assert (!private_vision_contains (self, object));
			private_vision_insert (self, object);
		}
	}
	else
	{
		if (private_vision_contains (self, object))
			private_vision_remove (self, object);
	}

	return 1;
}

/*****************************************************************************/

static void
private_vision_clear (LISerClient* self)
{
	// FIXME: Send packet.
	lialg_u32dic_clear (self->vision);
}

static int
private_vision_contains (LISerClient* self,
                         LIEngObject* object)
{
	return lialg_u32dic_find (self->vision, object->id) != NULL;
}

static void
private_vision_insert (LISerClient* self,
                       LIEngObject* object)
{
	LIArcWriter* writer;

	if (object->sector == NULL)
		return;
	if (lieng_object_get_flags (object) & LIENG_OBJECT_FLAG_INVISIBLE)
		return;

	/* Insert to vision. */
	lialg_u32dic_insert (self->vision, object->id, object);

	/* Assign if player object. */
	if (object == self->object)
	{
		writer = liarc_writer_new_packet (LINET_SERVER_PACKET_ASSIGN);
		if (writer != NULL)
		{
			liarc_writer_append_uint32 (writer, object->id);
			liarc_writer_append_uint32 (writer, 0); /* FIXME: Flags not supported anymore. */
			liser_client_send (self, writer, GRAPPLE_RELIABLE);
			liarc_writer_free (writer);
		}
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->server->callbacks, self->server, "vision-show", lical_marshal_DATA_PTR_PTR, self->object, object);
}

static void
private_vision_remove (LISerClient* self,
                       LIEngObject* object)
{
	/* Remove from vision. */
	lialg_u32dic_remove (self->vision, object->id);

	/* Invoke callbacks. */
	lical_callbacks_call (self->server->callbacks, self->server, "vision-hide", lical_marshal_DATA_PTR_PTR, self->object, object);
}

static void
private_vision_motion (LISerClient* self,
                       LIEngObject* object)
{
	LIArcWriter* writer;
	LIMatTransform transform;
	LIMatVector velocity;

	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_SIMULATE);
	if (writer == NULL)
		return;
	lieng_object_get_transform (object, &transform);
	lieng_object_get_velocity (object, &velocity);
	liarc_writer_append_uint32 (writer, object->id);
	liarc_writer_append_uint8 (writer, 0); /* FIXME */
	liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.x));
	liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.y));
	liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.z));
	liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.w));
	liarc_writer_append_float (writer, velocity.x);
	liarc_writer_append_float (writer, velocity.y);
	liarc_writer_append_float (writer, velocity.z);
	liarc_writer_append_float (writer, transform.position.x);
	liarc_writer_append_float (writer, transform.position.y);
	liarc_writer_append_float (writer, transform.position.z);
	liser_client_send (self, writer, 0);
	liarc_writer_free (writer);
}

static void
private_vision_update (LISerClient* self)
{
	float dist;
	LIAlgU32dicIter obj_iter;
	LIEngObjectIter eng_iter;
	LIMatTransform transform;

	if (self->object->sector == NULL)
		return;
	lieng_object_get_transform (self->object, &transform);

	/* Remove from vision. */
	LI_FOREACH_U32DIC (obj_iter, self->vision)
	{
		dist = lieng_object_get_distance (self->object, obj_iter.value);
		if (dist > self->radius)
			private_vision_remove (self, obj_iter.value);
	}

	/* Add to vision. */
	LIENG_FOREACH_OBJECT (eng_iter, self->object->engine, &transform.position, self->radius)
	{
		dist = lieng_object_get_distance (self->object, eng_iter.object);
		if (dist <= self->radius)
		{
			if (!private_vision_contains (self, obj_iter.value))
				private_vision_insert (self, obj_iter.value);
		}
	}
}

/** @} */
/** @} */
