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

#include <system/lips-system.h>
#include "client.h"

static int
private_assign (licliModule* module,
                liarcReader* reader);

static int
private_object_animation (licliModule* module,
                          liarcReader* reader);

static int
private_object_create (licliModule* module,
                       liarcReader* reader);

static int
private_object_destroy (licliModule* module,
                        liarcReader* reader);

static int
private_object_effect (licliModule* module,
                       liarcReader* reader);

static int
private_object_graphic (licliModule* module,
                        liarcReader* reader);

static int
private_object_simulate (licliModule* module,
                         liarcReader* reader);

static int
private_resources (licliModule* module,
                   liarcReader* reader);

static int
private_voxel_assign (licliModule* module,
                      liarcReader* reader);

static int
private_voxel_diff (licliModule* module,
                    liarcReader* reader);

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
                            liarcReader* reader)
{
	reader->pos = 1;
	switch (type)
	{
		case LINET_SERVER_PACKET_ASSIGN:
			private_assign (self, reader);
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
		case LIEXT_VOXEL_PACKET_ASSIGN:
			private_voxel_assign (self, reader);
			break;
		case LIEXT_VOXEL_PACKET_DIFF:
			private_voxel_diff (self, reader);
			return 0;
	}

	reader->pos = 1;
	return 1;
}

/*****************************************************************************/

static int
private_assign (licliModule* module,
                liarcReader* reader)
{
	lialgU32dicIter iter;
	licliObject* object;

	if (!liarc_reader_get_uint32 (reader, &module->network->id) ||
	    !liarc_reader_get_uint32 (reader, &module->network->features) ||
	    !liarc_reader_check_end (reader))
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
private_object_animation (licliModule* module,
                          liarcReader* reader)
{
	float priority;
	uint8_t channel;
	uint8_t permanent;
	uint32_t id;
	uint16_t animation;
	liengObject* object;

	/* Parse the packet. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint16 (reader, &animation) ||
	    !liarc_reader_get_uint8 (reader, &channel) ||
	    !liarc_reader_get_uint8 (reader, &permanent) ||
	    !liarc_reader_get_float (reader, &priority) ||
	    !liarc_reader_check_end (reader))
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
                       liarcReader* reader)
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
	object = lieng_engine_find_object (module->engine, id);
	if (object == NULL)
	{
		object = licli_object_new (module, id, flags);
		if (object == NULL)
			return 0;
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
private_object_destroy (licliModule* module,
                        liarcReader* reader)
{
	uint32_t id;
	liengObject* object;

	/* Parse the packet. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_check_end (reader))
		return 0;

	/* Destroy the object. */
	object = lieng_engine_find_object (module->engine, id);
	if (object == NULL)
		return 1;
	lieng_object_set_selected (object, 0);
	lieng_object_set_realized (object, 0);

	return 1;
}

static int
private_object_effect (licliModule* module,
                       liarcReader* reader)
{
	uint32_t id;
	uint16_t effect;
	uint16_t flags;
	liengObject* object;

	/* Parse the packet. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint16 (reader, &effect) ||
	    !liarc_reader_get_uint16 (reader, &flags) ||
	    !liarc_reader_check_end (reader))
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
                        liarcReader* reader)
{
	uint32_t id;
	uint16_t graphic;
	liengObject* object;

	/* Parse the packet. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint16 (reader, &graphic) ||
	    !liarc_reader_check_end (reader))
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
                         liarcReader* reader)
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
	memset (&controls, 0, sizeof (licliControls));
	if (flags & LI_CONTROL_MOVE_FRONT)
		controls.move += 1.0;
	if (flags & LI_CONTROL_MOVE_BACK)
		controls.move -= 1.0f;

	/* Change the simulation of the object. */
	object = licli_module_find_object (module, id);
	if (object == NULL)
		return 1;
	if (id == module->network->id)
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
private_resources (licliModule* module,
                   liarcReader* reader)
{
	lieng_engine_load_resources (module->engine, reader);

	return 1;
}

static int
private_voxel_assign (licliModule* module,
                      liarcReader* reader)
{
	livoxMaterial* material;

	while (!liarc_reader_check_end (reader))
	{
		material = livox_material_new_from_stream (reader);
		if (material == NULL)
			return 0;
		if (!livox_manager_insert_material (module->voxels, material))
			livox_material_free (material);
	}

	return 1;
}

static int
private_voxel_diff (licliModule* module,
                    liarcReader* reader)
{
	uint8_t sectorx;
	uint8_t sectory;
	uint8_t sectorz;
	uint16_t blockid;
	livoxBlock* block;
	livoxSector* sector;

	while (!liarc_reader_check_end (reader))
	{
		/* Read block offset. */
		if (!liarc_reader_get_uint8 (reader, &sectorx) ||
		    !liarc_reader_get_uint8 (reader, &sectory) ||
		    !liarc_reader_get_uint8 (reader, &sectorz) ||
		    !liarc_reader_get_uint16 (reader, &blockid))
			return 0;
		if (blockid >= LIVOX_BLOCKS_PER_SECTOR)
			return 0;

		/* Find block. */
		sector = livox_manager_create_sector (module->voxels,
			LIENG_SECTOR_INDEX (sectorx, sectory, sectorz));
		if (sector == NULL)
			return 0;
		block = livox_sector_get_block (sector, blockid);

		/* Read block data. */
		if (!livox_block_read (block, module->voxels, reader))
			return 0;
		if (livox_block_get_dirty (block))
			livox_sector_set_dirty (sector, 1);
	}

	return 1;
}
