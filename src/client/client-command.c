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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliCommand Command
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "client-event.h"

static int
private_assign (licliModule* module,
                liReader*    reader);

static int
private_chat (licliModule* module,
              liReader*    reader);

static int
private_custom (licliModule* module,
                liReader*    reader);

static int
private_object_animation (licliModule* module,
                          liReader*    reader);

static int
private_object_create (licliModule* module,
                       liReader*    reader);

static int
private_object_destroy (licliModule* module,
                        liReader*    reader);

static int
private_object_effect (licliModule* module,
                       liReader*    reader);

static int
private_object_graphic (licliModule* module,
                        liReader*    reader);

static int
private_object_simulate (licliModule* module,
                         liReader*    reader);

static int
private_resources (licliModule* module,
                   liReader*    reader);

static int
private_voxel_box (licliModule* module,
                   liReader*    reader);

static int
private_voxel_diff (licliModule* module,
                    liReader*    reader);

static int
private_voxel_sphere (licliModule* module,
                      liReader*    reader);

/*****************************************************************************/

/**
 * \brief Handles a core network packet.
 *
 * \param self Module.
 * \param type Packet type.
 * \param reader Packet reader.
 * \return Zero if handled, nonzero if should be passed to other packet handlers.
 */
int
licli_module_handle_packet (licliModule* self,
                            int          type,
                            liReader*    reader)
{
	switch (type)
	{
		case LINET_SERVER_PACKET_ASSIGN:
			private_assign (self, reader);
			return 0;
		case LINET_SERVER_PACKET_CHAT:
			private_chat (self, reader);
			return 0;
		case LINET_SERVER_PACKET_OBJECT_ANIMATION:
			private_object_animation (self, reader);
			return 0;
		case LINET_SERVER_PACKET_OBJECT_CREATE:
			private_object_create (self, reader);
			return 0;
		case LINET_SERVER_PACKET_OBJECT_DESTROY:
			private_object_destroy (self, reader);
			return 0;
		case LINET_SERVER_PACKET_OBJECT_EFFECT:
			private_object_effect (self, reader);
			return 0;
		case LINET_SERVER_PACKET_OBJECT_GRAPHIC:
			private_object_graphic (self, reader);
			return 0;
		case LINET_SERVER_PACKET_OBJECT_SIMULATE:
			private_object_simulate (self, reader);
			return 0;
		case LINET_SERVER_PACKET_RESOURCES:
			private_resources (self, reader);
			return 0;
		case LIEXT_VOXEL_PACKET_BOX:
			private_voxel_box (self, reader);
			return 0;
		case LIEXT_VOXEL_PACKET_DIFF:
			private_voxel_diff (self, reader);
			return 0;
		case LIEXT_VOXEL_PACKET_SPHERE:
			private_voxel_sphere (self, reader);
			return 0;
		case LINET_SERVER_PACKET_CUSTOM:
			private_custom (self, reader);
			return 0;
	}

	return 1;
}

/*****************************************************************************/

static int
private_assign (licliModule* module,
                liReader*    reader)
{
	lialgU32dicIter iter;
	licliObject* object;

	if (!li_reader_get_uint32 (reader, &module->network->id) ||
	    !li_reader_get_uint32 (reader, &module->network->features) ||
	    !li_reader_check_end (reader))
		return 0;

	/* Clear scene. */
	LI_FOREACH_U32DIC (iter, module->engine->objects)
	{
		object = LICLI_OBJECT (iter.value);
		if (object != NULL)
			lieng_object_set_realized (iter.value, 0);
	}

	return 1;
}

static int
private_chat (licliModule* module,
              liReader*    reader)
{
	uint32_t id;
	char* message = NULL;
	liengObject* object;

	if (!li_reader_get_uint32 (reader, &id) ||
	    !li_reader_get_text (reader, "", &message) ||
	    !li_reader_check_end (reader))
	{
		free (message);
		return 0;
	}

	/* FIXME */
	object = licli_module_find_object (module, id);
	if (object != NULL)
		licli_object_set_speech (object, message);
	else
		printf ("FIXME: %s: \"%s\"\n", "<missing>", message);

	lical_callbacks_call (module->engine->callbacks, LICLI_CALLBACK_CHAT, object, message);

	licli_module_event (module, LICLI_EVENT_TYPE_CHAT,
		"client", LISCR_TYPE_INT, id,
		"message", LISCR_TYPE_STRING, message, NULL);
	free (message);
	return 1;
}

static int
private_custom (licliModule* module,
                liReader*    reader)
{
	licli_module_event (module, LICLI_EVENT_TYPE_PACKET,
		"message", LISCR_TYPE_INT, (int)(((uint8_t*) reader->buffer)[0]),
		"packet", LICOM_SCRIPT_PACKET, reader, NULL);
	return 1;
}

static int
private_object_animation (licliModule* module,
                          liReader*    reader)
{
	float priority;
	uint8_t channel;
	uint8_t permanent;
	uint32_t id;
	uint16_t animation;
	liengObject* object;

	/* Parse the packet. */
	if (!li_reader_get_uint32 (reader, &id) ||
	    !li_reader_get_uint16 (reader, &animation) ||
	    !li_reader_get_uint8 (reader, &channel) ||
	    !li_reader_get_uint8 (reader, &permanent) ||
	    !li_reader_get_float (reader, &priority) ||
	    !li_reader_check_end (reader))
		return 0;

	/* Change the animations of the object. */
	object = licli_module_find_object (module, id);
	if (object == NULL)
		return 1;
	licli_object_set_animation (object, animation, channel, permanent, priority);

	return 1;
}

static int
private_object_create (licliModule* module,
                       liReader*    reader)
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
	liengObject* object;
	limatQuaternion rotation;
	limatTransform transform;
	limatVector position;
	limatVector velocity;

	/* Parse the packet. */
	if (!li_reader_get_uint32 (reader, &id) ||
	    !li_reader_get_uint16 (reader, &graphic) ||
	    !li_reader_get_uint8 (reader, &flags) ||
	    !li_reader_get_int8 (reader, &x) ||
	    !li_reader_get_int8 (reader, &y) ||
	    !li_reader_get_int8 (reader, &z) ||
	    !li_reader_get_int8 (reader, &w) ||
	    !li_reader_get_float (reader, &velocity.x) ||
	    !li_reader_get_float (reader, &velocity.y) ||
	    !li_reader_get_float (reader, &velocity.z) ||
	    !li_reader_get_float (reader, &position.x) ||
	    !li_reader_get_float (reader, &position.y) ||
	    !li_reader_get_float (reader, &position.z) ||
	    !li_reader_get_uint8 (reader, &anims))
		return 0;

	/* Create an object. */
	object = licli_object_new (module, id, flags);
	if (object == NULL)
		return 0;
	lieng_object_ref (object, 1);
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
		if (!li_reader_get_uint16 (reader, &anim) ||
		    !li_reader_get_uint8 (reader, &channel) ||
		    !li_reader_get_uint8 (reader, &flags) ||
		    !li_reader_get_float (reader, &priority))
			return 0;
		licli_object_set_animation (object, anim, channel, flags, priority);
	}
	if (!li_reader_check_end (reader))
		return 0;

	return 1;
}

static int
private_object_destroy (licliModule* module,
                        liReader*    reader)
{
	uint32_t id;
	liengObject* object;

	/* Parse the packet. */
	if (!li_reader_get_uint32 (reader, &id) ||
	    !li_reader_check_end (reader))
		return 0;

	/* Destroy the object. */
	object = lieng_engine_find_object (module->engine, id);
	if (object == NULL)
		return 1;
	lieng_object_set_realized (object, 0);
	lieng_object_set_selected (object, 0);
	lieng_object_ref (object, -1);

	return 1;
}

static int
private_object_effect (licliModule* module,
                       liReader*    reader)
{
	uint32_t id;
	uint16_t effect;
	uint16_t flags;
	liengObject* object;

	/* Parse the packet. */
	if (!li_reader_get_uint32 (reader, &id) ||
	    !li_reader_get_uint16 (reader, &effect) ||
	    !li_reader_get_uint16 (reader, &flags) ||
	    !li_reader_check_end (reader))
		return 0;

	/* Change the graphics of the object. */
	object = licli_module_find_object (module, id);
	if (object == NULL)
		return 1;
	licli_object_set_effect (object, effect, flags);

	return 1;
}

static int
private_object_graphic (licliModule* module,
                        liReader*    reader)
{
	uint32_t id;
	uint16_t graphic;
	liengObject* object;

	/* Parse the packet. */
	if (!li_reader_get_uint32 (reader, &id) ||
	    !li_reader_get_uint16 (reader, &graphic) ||
	    !li_reader_check_end (reader))
		return 0;

	/* Change the graphics of the object. */
	object = licli_module_find_object (module, id);
	if (object == NULL)
		return 1;
	lieng_object_set_model_code (object, graphic);

	return 1;
}

static int
private_object_simulate (licliModule* module,
                         liReader*    reader)
{
	int8_t x;
	int8_t y;
	int8_t z;
	int8_t w;
	uint8_t flags;
	uint32_t id;
	licliControls controls;
	liengObject* object;
	limatQuaternion rotation;
	limatTransform transform;
	limatVector position;
	limatVector velocity;

	/* Parse the packet. */
	if (!li_reader_get_uint32 (reader, &id) ||
	    !li_reader_get_uint8 (reader, &flags) ||
	    !li_reader_get_int8 (reader, &x) ||
	    !li_reader_get_int8 (reader, &y) ||
	    !li_reader_get_int8 (reader, &z) ||
	    !li_reader_get_int8 (reader, &w) ||
	    !li_reader_get_float (reader, &velocity.x) ||
	    !li_reader_get_float (reader, &velocity.y) ||
	    !li_reader_get_float (reader, &velocity.z) ||
	    !li_reader_get_float (reader, &position.x) ||
	    !li_reader_get_float (reader, &position.y) ||
	    !li_reader_get_float (reader, &position.z) ||
	    !li_reader_check_end (reader))
		return 0;

	/* FIXME: No analog. */
	memset (&controls, 0, sizeof (licliControls));
	if (flags & LI_CONTROL_MOVE_FRONT)
		controls.move += 1.0;
	if (flags & LI_CONTROL_MOVE_BACK)
		controls.move -= 1.0f;

	/* Change the simulation of the object. */
	object = licli_module_find_object (module, id);
	if (object == NULL)
		return 1;
	if (id != module->network->id)
		rotation = limat_quaternion_init (x / 127.0f, y / 127.0f, z / 127.0f, w / 127.0f);
	else
		rotation = LICLI_OBJECT (object)->curr.transform.rotation;
	transform = limat_transform_init (position, rotation);
	lieng_object_set_transform (object, &transform);
	lieng_object_set_velocity (object, &velocity);

	return 1;
}

static int
private_resources (licliModule* module,
                   liReader*    reader)
{
	lieng_engine_load_resources (module->engine, reader);

	return 1;
}

static int
private_voxel_box (licliModule* module,
                   liReader*    reader)
{
	uint32_t terrain;
	lialgU32dicIter iter;
	liengSector* sector;
	limatAabb aabb;
	limatAabb aabb1;

	/* Parse the packet. */
	if (!li_reader_get_float (reader, &aabb.min.x) ||
	    !li_reader_get_float (reader, &aabb.min.y) ||
	    !li_reader_get_float (reader, &aabb.min.z) ||
	    !li_reader_get_float (reader, &aabb.max.x) ||
	    !li_reader_get_float (reader, &aabb.max.y) ||
	    !li_reader_get_float (reader, &aabb.max.z) ||
	    !li_reader_get_uint32 (reader, &terrain) ||
	    !li_reader_check_end (reader))
		return 0;

	/* Edit terrain. */
	LI_FOREACH_U32DIC (iter, module->engine->sectors)
	{
		sector = iter.value;
		aabb1.min = limat_vector_subtract (aabb.min, sector->origin);
		aabb1.max = limat_vector_subtract (aabb.max, sector->origin);
		lieng_sector_fill_aabb (sector, &aabb1, terrain);
	}

	return 1;
}

static int
private_voxel_diff (licliModule* module,
                    liReader*    reader)
{
	uint16_t blockid;
	uint32_t sectorid;
	liengBlock* block;
	liengSector* sector;

	while (!li_reader_check_end (reader))
	{
		/* Read block offset. */
		if (!li_reader_get_uint32 (reader, &sectorid) ||
			!li_reader_get_uint16 (reader, &blockid))
			return 0;
#warning FIXME: Validate sector id.
		if (blockid >= LIENG_BLOCKS_PER_SECTOR)
			return 0;

		/* Find block. */
		sector = lieng_engine_create_sector (module->engine, sectorid);
		if (sector == NULL)
			return 0;
		block = sector->blocks + blockid;

		/* Read block data. */
		if (!lieng_block_read (block, reader))
			return 0;
		if (block->rebuild)
			sector->rebuild = 1;
	}

	return 1;
}

static int
private_voxel_sphere (licliModule* module,
                      liReader*    reader)
{
	float radius;
	uint32_t terrain;
	lialgU32dicIter iter;
	liengSector* sector;
	limatVector center;
	limatVector center1;

	/* Parse the packet. */
	if (!li_reader_get_float (reader, &center.x) ||
	    !li_reader_get_float (reader, &center.y) ||
	    !li_reader_get_float (reader, &center.z) ||
	    !li_reader_get_float (reader, &radius) ||
	    !li_reader_get_uint32 (reader, &terrain) ||
	    !li_reader_check_end (reader))
		return 0;

	/* Edit terrain. */
	LI_FOREACH_U32DIC (iter, module->engine->sectors)
	{
		sector = iter.value;
		center1 = limat_vector_subtract (center, sector->origin);
		lieng_sector_fill_sphere (sector, &center1, radius, terrain);
	}

	return 1;
}
