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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliCommand Command
 * @{
 */

#include <lipsofsuna/system.h>
#include "client.h"

static int
private_assign (LICliClient* client,
                LIArcReader* reader);

static int
private_object_animation (LICliClient* client,
                          LIArcReader* reader);

static int
private_object_create (LICliClient* client,
                       LIArcReader* reader);

static int
private_object_destroy (LICliClient* client,
                        LIArcReader* reader);

static int
private_object_graphic (LICliClient* client,
                        LIArcReader* reader);

static int
private_object_simulate (LICliClient* client,
                         LIArcReader* reader);

static int
private_resources (LICliClient* client,
                   LIArcReader* reader);

/*****************************************************************************/

/**
 * \brief Handles a core network packet.
 *
 * \param self Client.
 * \param type Packet type.
 * \param reader Packet reader.
 * \return Zero if handled, nonzero if should be passed to other packet handlers.
 */
int
licli_client_handle_packet (LICliClient* self,
                            int          type,
                            LIArcReader* reader)
{
	reader->pos = 1;
	switch (type)
	{
		case LINET_SERVER_PACKET_ASSIGN:
			private_assign (self, reader);
			break;
		case LINET_SERVER_PACKET_OBJECT_ANIMATION:
			private_object_animation (self, reader);
			break;
		case LINET_SERVER_PACKET_OBJECT_CREATE:
			private_object_create (self, reader);
			break;
		case LINET_SERVER_PACKET_OBJECT_DESTROY:
			private_object_destroy (self, reader);
			break;
		case LINET_SERVER_PACKET_OBJECT_GRAPHIC:
			private_object_graphic (self, reader);
			break;
		case LINET_SERVER_PACKET_OBJECT_SIMULATE:
			private_object_simulate (self, reader);
			break;
		case LINET_SERVER_PACKET_RESOURCES:
			private_resources (self, reader);
			break;
	}

	reader->pos = 1;
	return 1;
}

/*****************************************************************************/

static int
private_assign (LICliClient* client,
                LIArcReader* reader)
{
	uint32_t id;
	uint32_t features;
	LIAlgU32dicIter iter;

	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint32 (reader, &features) ||
	    !liarc_reader_check_end (reader))
		return 0;
	client->network->id = id;
	client->network->features = features;

	/* Clear scene. */
	LIALG_U32DIC_FOREACH (iter, client->engine->objects)
	{
		if (LICLI_IS_CLIENT_OBJECT (iter.value))
			lieng_object_set_realized (iter.value, 0);
	}

	return 1;
}

static int
private_object_animation (LICliClient* client,
                          LIArcReader* reader)
{
	float priority;
	uint8_t channel;
	uint8_t permanent;
	uint32_t id;
	uint16_t animation;
	LIEngObject* object;

	/* Parse the packet. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint16 (reader, &animation) ||
	    !liarc_reader_get_uint8 (reader, &channel) ||
	    !liarc_reader_get_uint8 (reader, &permanent) ||
	    !liarc_reader_get_float (reader, &priority) ||
	    !liarc_reader_check_end (reader))
		return 0;

	/* Change the animations of the object. */
	object = lieng_engine_find_object (client->engine, id);
	if (object == NULL)
		return 1;
	licli_object_set_animation (object, animation, channel, permanent, priority);

	return 1;
}

static int
private_object_create (LICliClient* client,
                       LIArcReader* reader)
{
	int i;
	float priority;
	int8_t x;
	int8_t y;
	int8_t z;
	int8_t w;
	uint8_t anims;
	uint8_t channel;
	uint8_t flags;
	uint16_t anim;
	uint16_t graphic;
	uint32_t id;
	LIEngObject* object;
	LIMatQuaternion rotation;
	LIMatTransform transform;
	LIMatVector position;
	LIMatVector velocity;

	/* Parse the packet. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint16 (reader, &graphic) ||
	    !liarc_reader_get_uint8 (reader, &flags) ||
	    !liarc_reader_get_int8 (reader, &x) ||
	    !liarc_reader_get_int8 (reader, &y) ||
	    !liarc_reader_get_int8 (reader, &z) ||
	    !liarc_reader_get_int8 (reader, &w) ||
	    !liarc_reader_get_float (reader, &velocity.x) ||
	    !liarc_reader_get_float (reader, &velocity.y) ||
	    !liarc_reader_get_float (reader, &velocity.z) ||
	    !liarc_reader_get_float (reader, &position.x) ||
	    !liarc_reader_get_float (reader, &position.y) ||
	    !liarc_reader_get_float (reader, &position.z) ||
	    !liarc_reader_get_uint8 (reader, &anims))
		return 0;

	/* Create an object. */
	object = lieng_engine_find_object (client->engine, id);
	if (object == NULL)
	{
		object = lieng_object_new (client->engine, NULL, LIPHY_CONTROL_MODE_STATIC, id);
		if (object == NULL)
			return 0;
		licli_object_set_flags (object, flags);
	}
	lieng_object_set_model_code (object, graphic);

	/* Set transformation. */
	rotation = limat_quaternion_init (x / 127.0f, y / 127.0f, z / 127.0f, w / 127.0f);
	transform = limat_transform_init (position, rotation);
	lieng_object_set_transform (object, &transform);
	lieng_object_set_velocity (object, &velocity);
	lieng_object_set_realized (object, 1);

	/* Set animations. */
	for (i = 0 ; i < anims ; i++)
	{
		if (!liarc_reader_get_uint16 (reader, &anim) ||
		    !liarc_reader_get_uint8 (reader, &channel) ||
		    !liarc_reader_get_uint8 (reader, &flags) ||
		    !liarc_reader_get_float (reader, &priority))
			return 0;
		licli_object_set_animation (object, anim, channel, flags, priority);
	}
	if (!liarc_reader_check_end (reader))
		return 0;

	return 1;
}

static int
private_object_destroy (LICliClient* client,
                        LIArcReader* reader)
{
	uint32_t id;
	LIEngObject* object;

	/* Parse the packet. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_check_end (reader))
		return 0;

	/* Destroy the object. */
	object = lieng_engine_find_object (client->engine, id);
	if (object == NULL)
		return 1;
	lieng_object_set_selected (object, 0);
	lieng_object_set_realized (object, 0);

	return 1;
}

static int
private_object_graphic (LICliClient* client,
                        LIArcReader* reader)
{
	uint32_t id;
	uint16_t graphic;
	LIEngObject* object;

	/* Parse the packet. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint16 (reader, &graphic) ||
	    !liarc_reader_check_end (reader))
		return 0;

	/* Change the graphics of the object. */
	object = lieng_engine_find_object (client->engine, id);
	if (object == NULL)
		return 1;
	lieng_object_set_model_code (object, graphic);

	return 1;
}

static int
private_object_simulate (LICliClient* client,
                         LIArcReader* reader)
{
	int8_t x;
	int8_t y;
	int8_t z;
	int8_t w;
	uint8_t flags;
	uint32_t id;
	LICliControls controls;
	LIEngObject* object;
	LIMatQuaternion rotation;
	LIMatTransform transform;
	LIMatVector position;
	LIMatVector velocity;

	/* Parse the packet. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint8 (reader, &flags) ||
	    !liarc_reader_get_int8 (reader, &x) ||
	    !liarc_reader_get_int8 (reader, &y) ||
	    !liarc_reader_get_int8 (reader, &z) ||
	    !liarc_reader_get_int8 (reader, &w) ||
	    !liarc_reader_get_float (reader, &velocity.x) ||
	    !liarc_reader_get_float (reader, &velocity.y) ||
	    !liarc_reader_get_float (reader, &velocity.z) ||
	    !liarc_reader_get_float (reader, &position.x) ||
	    !liarc_reader_get_float (reader, &position.y) ||
	    !liarc_reader_get_float (reader, &position.z) ||
	    !liarc_reader_check_end (reader))
		return 0;

	/* FIXME: No analog. */
	memset (&controls, 0, sizeof (LICliControls));
	if (flags & LINET_CONTROL_MOVE_FRONT)
		controls.move += 1.0;
	if (flags & LINET_CONTROL_MOVE_BACK)
		controls.move -= 1.0f;

	/* Change the simulation of the object. */
	object = lieng_engine_find_object (client->engine, id);
	if (object == NULL)
		return 1;
	if (id == client->network->id)
	{
		lieng_object_get_transform (object, &transform);
		rotation = transform.rotation;
	}
	else
		rotation = limat_quaternion_init (x / 127.0f, y / 127.0f, z / 127.0f, w / 127.0f);
	transform = limat_transform_init (position, rotation);
	lieng_object_set_transform (object, &transform);
	lieng_object_set_velocity (object, &velocity);

	return 1;
}

static int
private_resources (LICliClient* client,
                   LIArcReader* reader)
{
	lieng_engine_load_resources (client->engine, reader);

	return 1;
}
