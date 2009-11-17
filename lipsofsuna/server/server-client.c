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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvClient Client
 * @{
 */

#include <network/lips-network.h>
#include <system/lips-system.h>
#include "server.h"
#include "server-callbacks.h"
#include "server-client.h"
#include "server-script.h"

#define LISRV_CLIENT_DEFAULT_RADIUS 24
#define LISRV_CLIENT_LOAD_RADIUS 24

static void
private_callbacks_setup (lisrvClient* self,
                         liengEngine* engine);

static void
private_callbacks_clear (lisrvClient* self,
                         liengEngine* engine);

static int
private_object_animation (lisrvClient*   self,
                          liengObject*   object,
                          lisrvAniminfo* info);

static int
private_object_model (lisrvClient* self,
                      liengObject* object,
                      liengModel*  model);

static int
private_object_motion (lisrvClient* self,
                       liengObject* object);

static int
private_object_sample (lisrvClient* self,
                       liengObject* object,
                       liengSample* sample,
                       int          flags);

static int
private_object_visibility (lisrvClient* self,
                           liengObject* object,
                           int          visible);

static void
private_vision_clear (lisrvClient*  self);

static int
private_vision_contains (lisrvClient* self,
                         liengObject* object);

static void
private_vision_insert (lisrvClient* self,
                       liengObject* object);

static void
private_vision_remove (lisrvClient* self,
                       liengObject* object);

static void
private_vision_motion (lisrvClient* self,
                       liengObject* object);

static void
private_vision_update (lisrvClient* self);

/*****************************************************************************/

/**
 * \brief Frees the client.
 *
 * Only called by #lisrv_network_update.
 *
 * \param self Client.
 */
lisrvClient*
lisrv_client_new (lisrvServer* server,
                  liengObject* object,
                  grapple_user user)
{
	lisrvClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lisrvClient));
	if (self == NULL)
		return NULL;
	self->server = server;
	self->radius = LISRV_CLIENT_DEFAULT_RADIUS;
	self->network.user = user;

	/* Allocate vision. */
	self->vision = lialg_u32dic_new ();
	if (self->vision == NULL)
		goto error;

	/* Assign object. */
	if (!lisrv_client_set_object (self, object))
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
 * Only called by #lisrv_object_disconnect.
 *
 * \param self Client.
 */
void
lisrv_client_free (lisrvClient* self)
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
 * Only called by #lisrv_object_set_client.
 *
 * \param self Client.
 * \param object Object.
 * \return Nonzero on success.
 */
int
lisrv_client_set_object (lisrvClient* self,
                         liengObject* value)
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
lisrv_client_send (lisrvClient* self,
                   liarcWriter* writer,
                   int          flags)
{
	grapple_server_send (self->server->network->socket, self->network.user, flags,
		liarc_writer_get_buffer (writer),
		liarc_writer_get_length (writer));
}

int
lisrv_client_get_near (const lisrvClient* self,
                       const liengObject* object)
{
	return lialg_u32dic_find (self->vision, object->id) != NULL;
}

/*****************************************************************************/

static void
private_callbacks_setup (lisrvClient* self,
                         liengEngine* engine)
{
	lieng_engine_insert_call (engine, LISRV_CALLBACK_OBJECT_ANIMATION, 0, private_object_animation, self, self->calls + 0);
	lieng_engine_insert_call (engine, LISRV_CALLBACK_OBJECT_SAMPLE, 0, private_object_sample, self, self->calls + 1);
	lieng_engine_insert_call (engine, LISRV_CALLBACK_OBJECT_MODEL, 0, private_object_model, self, self->calls + 2);
	lieng_engine_insert_call (engine, LISRV_CALLBACK_OBJECT_MOTION, 0, private_object_motion, self, self->calls + 3);
	lieng_engine_insert_call (engine, LISRV_CALLBACK_OBJECT_VISIBILITY, 0, private_object_visibility, self, self->calls + 4);
}

static void
private_callbacks_clear (lisrvClient* self,
                         liengEngine* engine)
{
	lieng_engine_remove_calls (engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));
}

/*****************************************************************************/

static int
private_object_animation (lisrvClient*   self,
                          liengObject*   object,
                          lisrvAniminfo* info)
{
	liarcWriter* writer;

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
	lisrv_client_send (self, writer, 0);
	liarc_writer_free (writer);

	return 1;
}

static int
private_object_model (lisrvClient* self,
                      liengObject* object,
                      liengModel*  model)
{
	liarcWriter* writer;

	/* Send to client if seen. */
	if (!private_vision_contains (self, object))
		return 1;
	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_GRAPHIC);
	if (writer == NULL)
		return 1;
	liarc_writer_append_uint32 (writer, object->id);
	liarc_writer_append_uint16 (writer, lieng_object_get_model_code (object));
	lisrv_client_send (self, writer, GRAPPLE_RELIABLE);
	liarc_writer_free (writer);

	return 1;
}

static int
private_object_motion (lisrvClient* self,
                       liengObject* object)
{
	float dist;
	lialgRange range;
	lialgRangeIter iter;
	limatTransform transform;

	/* Make sure nearby sectors are loaded when self moved. */
	if (object == self->object)
	{
		lieng_object_get_transform (self->object, &transform);
		range = lialg_range_new_from_sphere (&transform.position, 
			LISRV_CLIENT_LOAD_RADIUS, LIENG_SECTOR_WIDTH);
		range = lialg_range_clamp (range, 0, 255);
		LIALG_RANGE_FOREACH (iter, range)
		{
			lieng_engine_load_sector (self->server->engine, iter.index);
		}
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
private_object_sample (lisrvClient* self,
                       liengObject* object,
                       liengSample* sample,
                       int          flags)
{
	liarcWriter* writer;

	/* Send to client if seen. */
	if (!private_vision_contains (self, object))
		return 1;
	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_EFFECT);
	if (writer == NULL)
		return 1;
	liarc_writer_append_uint32 (writer, object->id);
	liarc_writer_append_uint16 (writer, sample->id);
	liarc_writer_append_uint16 (writer, flags);
	lisrv_client_send (self, writer, 0);
	liarc_writer_free (writer);

	return 1;
}

static int
private_object_visibility (lisrvClient* self,
                           liengObject* object,
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
private_vision_clear (lisrvClient* self)
{
	// FIXME: Send packet.
	lialg_u32dic_clear (self->vision);
}

static int
private_vision_contains (lisrvClient* self,
                         liengObject* object)
{
	return lialg_u32dic_find (self->vision, object->id) != NULL;
}

static void
private_vision_insert (lisrvClient* self,
                       liengObject* object)
{
	liarcWriter* writer;

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
			lisrv_client_send (self, writer, GRAPPLE_RELIABLE);
			liarc_writer_free (writer);
		}
	}

	/* Invoke callbacks. */
	lieng_engine_call (self->server->engine, LISRV_CALLBACK_VISION_SHOW, self->object, object);
}

static void
private_vision_remove (lisrvClient* self,
                       liengObject* object)
{
	/* Remove from vision. */
	lialg_u32dic_remove (self->vision, object->id);

	/* Invoke callbacks. */
	lieng_engine_call (self->server->engine, LISRV_CALLBACK_VISION_HIDE, self->object, object);
}

static void
private_vision_motion (lisrvClient* self,
                       liengObject* object)
{
	liarcWriter* writer;
	limatTransform transform;
	limatVector velocity;

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
	lisrv_client_send (self, writer, 0);
	liarc_writer_free (writer);
}

static void
private_vision_update (lisrvClient* self)
{
	float dist;
	lialgU32dicIter obj_iter;
	lialgRange range;
	lialgRangeIter rangeiter;
	liengSector* sector;
	liengObject* object = self->object;

	if (object->sector == NULL)
		return;
	sector = object->sector;
	range = lialg_range_new (sector->x, sector->y, sector->z, 1);
	range = lialg_range_clamp (range, 0, 255);

	/* Remove from vision. */
	LI_FOREACH_U32DIC (obj_iter, self->vision)
	{
		dist = lieng_object_get_distance (object, obj_iter.value);
		if (dist > self->radius)
			private_vision_remove (self, obj_iter.value);
	}

	/* Add to vision. */
	LIALG_RANGE_FOREACH (rangeiter, range)
	{
		sector = lieng_engine_find_sector (object->engine, rangeiter.index);
		if (sector == NULL)
			continue;
		LI_FOREACH_U32DIC (obj_iter, sector->objects)
		{
			dist = lieng_object_get_distance (object, obj_iter.value);
			if (dist <= self->radius)
			{
				if (!private_vision_contains (self, obj_iter.value))
					private_vision_insert (self, obj_iter.value);
			}
		}
	}
}

/** @} */
/** @} */
