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
 * \addtogroup lisrvObject Object
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <network/lips-network.h>
#include <physics/lips-physics.h>
#include "server.h"
#include "server-callbacks.h"
#include "server-client.h"
#include "server-object.h"
#include "server-observer.h"
#include "server-script.h"

#define LISRV_OBJECT_VERSION 0

static int
private_read_block (liengObject*    self,
                    liarcSerialize* serialize);

/*****************************************************************************/

/**
 * \brief Creates a new server object.
 *
 * \param engine Engine.
 * \param model Model or NULL.
 * \param shape_mode Collision shape type.
 * \param control_mode Interaction type.
 * \param id Object number or zero.
 * \param ptr Data passed to engine.
 * \return New object or NULL.
 */
liengObject*
lisrv_object_new (liengEngine*     engine,
                  liengModel*      model,
                  liphyShapeMode   shape_mode,
                  liphyControlMode control_mode,
                  uint32_t         id,
                  void*            ptr)
{
	liengObject* self;
	lisrvObject* data;
	lisrvServer* server = lieng_engine_get_userdata (engine, LIENG_DATA_SERVER);

	/* Allocate engine data. */
	self = lieng_default_calls.lieng_object_new (engine, model, shape_mode, control_mode, id, ptr);
	if (self == NULL)
		return NULL;

	/* Allocate server data. */
	data = calloc (1, sizeof (lisrvObject));
	if (data == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}
	data->server = server;
	data->animations = lialg_u32dic_new ();
	if (data->animations == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}

	/* Extend engine object. */
	lieng_object_set_userdata (self, LIENG_DATA_SERVER, data);
	liphy_object_set_userdata (self->physics, self);

	/* Allocate script data. */
	self->script = liscr_data_new (server->script, self, LICOM_SCRIPT_OBJECT);
	if (self->script == NULL)
		goto error;
	liscr_data_unref (self->script, NULL);

	return self;

error:
	lieng_object_free (self);
	return NULL;
}

/**
 * \brief Marks the object as deleted.
 *
 * \param self Object.
 */
void
lisrv_object_free (liengObject* self)
{
	lialgU32dicIter iter;
	lisrvObject* data = LISRV_OBJECT (self);

	/* Free client. */
	lisrv_object_disconnect (self);

	/* Unrealize before server data is freed. */
	lieng_object_set_realized (self, 0);

	/* Free server data. */
	if (data != NULL)
	{
		LI_FOREACH_U32DIC (iter, data->animations)
			free (iter.value);
		lialg_u32dic_free (data->animations);
		free (data->name);
		free (data);
	}

	/* Free engine data. */
	lieng_default_calls.lieng_object_free (self);
}

/**
 * \brief Plays back an animation.
 *
 * \param self Object.
 * \param name Animation name or NULL.
 * \param channel Channel to use, -1 for the largest free channel number.
 * \param priority Blending priority.
 * \param permanent Nonzero if permanent, zero if temporary.
 * \return Nonzero on success.
 */
int
lisrv_object_animate (liengObject* self,
                      const char*  name,
                      int          channel,
                      float        priority,
                      int          permanent)
{
	liengAnimation* animation;
	lisrvAniminfo tmp;
	lisrvAniminfo* info;
	lisrvObject* data = LISRV_OBJECT (self);

	/* Find animation channel. */
	if (channel == -1)
	{
		for (channel = 254 ; channel > 0 ; channel--)
		{
			if (!lialg_u32dic_find (data->animations, channel))
				break;
		}
	}

	/* Find animation number. */
	if (name != NULL)
		animation = lieng_engine_find_animation_by_name (self->engine, name);
	else
		animation = NULL;

	/* Check for duplicate. */
	info = lialg_u32dic_find (data->animations, channel);
	if (info != NULL)
	{
		if (info->animation == animation &&
		    info->channel == channel &&
		    info->permanent == permanent &&
		    info->priority == priority)
			return 1;
	}

	/* Clear channel. */
	if (info != NULL)
	{
		lialg_u32dic_remove (data->animations, channel);
		free (info);
	}
	if (name != NULL && animation == NULL)
		return 0;

	/* Register channel if permanent. */
	if (permanent)
	{
		info = calloc (1, sizeof (lisrvAniminfo));
		if (info == NULL)
			return 0;
		info->animation = animation;
		info->channel = channel;
		info->permanent = permanent;
		info->priority = priority;
		if (!lialg_u32dic_insert (data->animations, channel, info))
		{
			free (info);
			return 0;
		}
	}

	/* Invoke callbacks. */
	if (lieng_object_get_realized (self))
	{
		tmp.animation = animation;
		tmp.channel = channel;
		tmp.permanent = permanent;
		tmp.priority = priority;
		lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_ANIMATION, self, &tmp);
	}

	return 1;
}

/**
 * \brief Disconnects the client controlling the object.
 *
 * \param self Object.
 */
void
lisrv_object_disconnect (liengObject* self)
{
	lisrvObject* data = LISRV_OBJECT (self);

	if (data->client != NULL)
	{
		lieng_engine_call (self->engine, LISRV_CALLBACK_CLIENT_LOGOUT, self);
		lisrv_client_free (data->client);
		data->client = NULL;
		lieng_object_ref (self, -1);
	}
}

/**
 * \brief Emit a sound or graphical effect.
 *
 * \param self Object.
 * \param value Effect name.
 * \param flags Effect flags.
 */
void
lisrv_object_effect (liengObject* self,
                     const char*  value,
                     int          flags)
{
	liengSample* sample;
	lisrvObject* data = LISRV_OBJECT (self);

	/* Find effect. */
	if (!lieng_object_get_realized (self))
		return;
	sample = lieng_resources_find_sample_by_name (data->server->engine->resources, value);
	if (sample == NULL)
		return;

	/* Invoke callbacks. */
	lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_SAMPLE, self, sample, flags);
}

/**
 * \brief Called by the engine every time the object moves.
 *
 * \param self Object.
 */
int
lisrv_object_moved (liengObject* self)
{
	/* Call base. */
	if (!lieng_default_calls.lieng_object_moved (self))
		return 0;

	/* Invoke callbacks. */
	lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_MOTION, self);

	return 1;
}

/**
 * \brief Serializes or deserializes the object.
 * 
 * \param self Object.
 * \param serialize Deserializer.
 * \return Nonzero on success.
 */
int
lisrv_object_serialize (liengObject*    self,
                        liarcSerialize* serialize)
{
	int i;
	int ret;
	char* type;
	uint32_t end;
	uint32_t blocks;
	uint32_t size;
	lialgU32dicIter iter;
	liarcWriter* engine;
	liarcWriter* server;
	liarcWriter* script;
	lisrvAniminfo* info;

	if (liarc_serialize_get_write (serialize))
	{
		/* Write engine. */
		if (!liarc_serialize_push (serialize))
			return 0;
		if (!lieng_default_calls.lieng_object_serialize (self, serialize))
			return 0;

		/* Write server. */
		if (!liarc_serialize_push (serialize))
			return 0;
		liarc_writer_append_uint8 (serialize->writer, LISRV_OBJECT_VERSION);
		liarc_writer_append_uint32 (serialize->writer, LISRV_OBJECT (self)->animations->size);
		LI_FOREACH_U32DIC (iter, LISRV_OBJECT (self)->animations)
		{
			info = iter.value;
			liarc_writer_append_string (serialize->writer, info->animation->name);
			liarc_writer_append_uint32 (serialize->writer, info->channel);
			liarc_writer_append_uint32 (serialize->writer, 0);
			liarc_writer_append_uint32 (serialize->writer, 0);
			liarc_writer_append_float (serialize->writer, info->priority);
			liarc_writer_append_float (serialize->writer, 0.0f);
			liarc_writer_append_float (serialize->writer, 0.0f);
			liarc_writer_append_float (serialize->writer, 0.0f);
		}
		if (serialize->writer->error)
			return 0;

		/* Write script. */
		if (!liarc_serialize_push (serialize))
			return 0;
		if (!liscr_data_write (self->script, serialize))
			return 0;

		/* Pack all to stream. */
		script = liarc_serialize_pop (serialize);
		server = liarc_serialize_pop (serialize);
		engine = liarc_serialize_pop (serialize);
		size = strlen ("engine") + 1 + 4 + 4 + engine->memory.length +
			strlen ("server") + 1 + 4 + server->memory.length +
			strlen ("script") + 1 + 4 + script->memory.length;
		ret = liarc_writer_append_string (serialize->writer, "object") &&
			  liarc_writer_append_nul (serialize->writer) &&
			  liarc_writer_append_uint32 (serialize->writer, size) &&
			  liarc_writer_append_uint32 (serialize->writer, 3) &&
			  liarc_writer_append_string (serialize->writer, "engine") &&
			  liarc_writer_append_nul (serialize->writer) &&
			  liarc_writer_append_uint32 (serialize->writer, engine->memory.length) &&
			  liarc_writer_append_raw (serialize->writer, engine->memory.buffer, engine->memory.length) &&
			  liarc_writer_append_string (serialize->writer, "server") &&
			  liarc_writer_append_nul (serialize->writer) &&
			  liarc_writer_append_uint32 (serialize->writer, server->memory.length) &&
			  liarc_writer_append_raw (serialize->writer, server->memory.buffer, server->memory.length) &&
			  liarc_writer_append_string (serialize->writer, "script") &&
			  liarc_writer_append_nul (serialize->writer) &&
			  liarc_writer_append_uint32 (serialize->writer, script->memory.length) &&
			  liarc_writer_append_raw (serialize->writer, script->memory.buffer, script->memory.length);
		liarc_writer_free (script);
		liarc_writer_free (server);
		liarc_writer_free (engine);

		return !serialize->writer->error;
	}
	else
	{
		/* Read header. */
		if (!li_reader_get_text (serialize->reader, "", &type))
			return 0;
		if (!li_reader_get_uint32 (serialize->reader, &size))
		{
			free (type);
			return 0;
		}
		end = li_reader_get_offset (serialize->reader) + size;

		/* Read blocks. */
		if (!strcmp (type, "object"))
		{
			if (!li_reader_get_uint32 (serialize->reader, &blocks))
			{
				free (type);
				return 0;
			}
			for (i = 0 ; i < blocks ; i++)
				private_read_block (self, serialize);
		}
		else
		{
			li_reader_set_offset (serialize->reader, end);
			free (type);
			return 1;
		}

		/* Check for read size. */
		if (li_reader_get_offset (serialize->reader) != end)
		{
			lisys_error_set (EINVAL, "size mismatch in block `%s' (%d/%d)",
				type, li_reader_get_offset (serialize->reader), end);
			li_reader_set_offset (serialize->reader, end);
			free (type);
			return 0;
		}

		free (type);
		return 1;
	}
}

int
lisrv_object_sees (const liengObject* self,
                   const liengObject* target)
{
	lisrvObject* data = LISRV_OBJECT (self);

	if (data->client == NULL)
		return 0;
	return (lialg_u32dic_find (data->client->vision, target->id) != NULL);
}

/**
 * \brief Solves path to the requested point.
 *
 * \param self Object.
 * \param target Target position vector.
 * \return New path or NULL if couldn't solve.
 */
liaiPath*
lisrv_object_solve_path (const liengObject* self,
                         const limatVector*    target)
{
#warning Path solving is disabled.
#if 0
	liaiWaypoint* start;
	liaiWaypoint* end;
	limatVector position;
	lisrvObject* data = LISRV_OBJECT (self);

	if (self->sector == NULL || self->sector->aisector == NULL)
		return NULL;

	lisrv_object_get_position (self, &position);
	start = liai_sector_find_waypoint (self->sector->aisector, &position);
	end = liai_sector_find_waypoint (self->sector->aisector, target);
	if (start == NULL || end == NULL)
		return NULL;

	return liai_path_solver_solve (server->helper.path_solver, start, end);
#endif
	return NULL;
}

/**
 * \brief Swaps the clients of the objects.
 *
 * \param self Object.
 * \param object Object.
 */
void
lisrv_object_swap (liengObject* self,
                   liengObject* object)
{
	lisrvClient* tmp;
	lisrvObject* data = LISRV_OBJECT (self);
	lisrvObject* data1 = LISRV_OBJECT (object);

	tmp = data->client;
	lisrv_object_set_client (self, data1->client);
	lisrv_object_set_client (object, tmp);
}

/**
 * \brief Sets the client controlling the object.
 *
 * \param self Object.
 * \param value Client.
 * \return Nonzero on success.
 */
int
lisrv_object_set_client (liengObject* self,
                         lisrvClient* value)
{
	lisrvObject* data = LISRV_OBJECT (self);

	if (data->client == value)
		return 1;
	data->client = value;
	if (value != NULL)
	{
		lieng_object_ref (self, 1);
		return lisrv_client_set_object (value, self);
	}
	else
	{
		lieng_object_ref (self, -1);
		return 1;
	}
}

/**
 * \brief Gets the model number of the object.
 *
 * \param self An object.
 * \return Model number.
 */
uint32_t
lisrv_object_get_model (const liengObject* self)
{
	return lieng_object_get_model_code (self);
}

/**
 * \brief Sets the model of an object.
 *
 * \param self Object.
 * \param value Model.
 * \return Nonzero on success.
 */
int
lisrv_object_set_model (liengObject* self,
                        liengModel*  value)
{
	if (!lieng_default_calls.lieng_object_set_model (self, value))
		return 0;

	/* Invoke callbacks. */
	lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_MODEL, self, value);

	return 1;
}

/**
 * \brief Gets the name of the object.
 *
 * \param self Object.
 * \return String guaranteed to not be NULL.
 */
const char*
lisrv_object_get_name (const liengObject* self)
{
	lisrvObject* data = LISRV_OBJECT (self);

	if (data->name == NULL)
		return "";
	return data->name;
}

/**
 * \brief Sets the name of the object.
 *
 * \param self Object.
 * \param value String.
 * \return Nonzero if successsful.
 */
int
lisrv_object_set_name (liengObject* self,
                       const char*  value)
{
	char* tmp;
	lisrvObject* data = LISRV_OBJECT (self);

	tmp = strdup (value);
	if (tmp == NULL)
		return 0;
	free (data->name);
	data->name = tmp;

	return 1;
}

/**
 * \brief Sets the world transformation of the object.
 *
 * This function takes care of moving the object between map sectors and
 * sending events. If you want to modify to the position of an object,
 * this function is basically the only valid way of doing it.
 *
 * If the object is not realized, the position is set but nothing else
 * happens. However, if the object is later realized, the position set
 * with this function will be used as its world position.
 *
 * \param self Object.
 * \param value Transformation.
 * \return Nonzero on success.
 */
int
lisrv_object_set_transform (liengObject*          self,
                            const limatTransform* value)
{
	/* Call base. */
	if (!lieng_default_calls.lieng_object_set_transform (self, value))
		return 0;

	return 1;
}

int
lisrv_object_set_realized (liengObject* self,
                           int          value)
{
	if (value == lieng_object_get_realized (self))
		return 1;
	if (value)
	{
		/* Call base. */
		if (!lieng_default_calls.lieng_object_set_realized (self, 1))
			return 0;

		/* Invoke callbacks. */
		lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_VISIBILITY, self, 1);
	}
	else
	{
		/* Invoke callbacks. */
		lieng_engine_call (self->engine, LISRV_CALLBACK_OBJECT_VISIBILITY, self, 0);

		/* Call base. */
		if (!lieng_default_calls.lieng_object_set_realized (self, 0))
			return 0;
	}

	return 1;
}

int
lisrv_object_get_valid (const liengObject* self)
{
	return !self->script->invalid;
}

/**
 * \brief Set the velocity vector of the object.
 *
 * \param self object
 * \param value Vector.
 * \return Nonzero on success.
 */
int
lisrv_object_set_velocity (liengObject*       self,
                           const limatVector* value)
{
	/* Call base. */
	if (!lieng_default_calls.lieng_object_set_velocity (self, value))
		return 0;

	return 1;
}

/*****************************************************************************/

static int
private_read_block (liengObject*    self,
                    liarcSerialize* serialize)
{
	char* type;
	char* name;
	float priority;
	uint8_t version;
	uint32_t i;
	uint32_t anims;
	uint32_t channel;
	uint32_t end;
	uint32_t pad;
	uint32_t size;
	liReader* reader = serialize->reader;

	/* Read block header. */
	if (!li_reader_get_text (reader, "", &type))
		return 0;
	if (!li_reader_get_uint32 (reader, &size))
	{
		free (type);
		return 0;
	}
	end = li_reader_get_offset (reader) + size;

	/* Read block data. */
	/* FIXME: Should prevent reading past block end. */
	if (!strcmp (type, "engine"))
	{
		/* Read engine. */
		if (!lieng_default_calls.lieng_object_serialize (self, serialize))
			goto error;
	}
	else if (!strcmp (type, "server"))
	{
		/* Read server. */
		if (!li_reader_get_uint8 (reader, &version) ||
			!li_reader_get_uint32 (reader, &anims))
			goto error;
		if (version != LISRV_OBJECT_VERSION)
		{
			lisys_error_set (EINVAL, "incorrect server object version");
			goto error;
		}
		for (i = 0 ; i < anims ; i++)
		{
			name = NULL;
			if (!li_reader_get_text (reader, "", &name) ||
				!li_reader_get_uint32 (reader, &channel) ||
				!li_reader_get_uint32 (reader, &pad) ||
				!li_reader_get_uint32 (reader, &pad) ||
				!li_reader_get_float (reader, &priority) ||
				!li_reader_get_uint32 (reader, &pad) ||
				!li_reader_get_uint32 (reader, &pad) ||
				!li_reader_get_uint32 (reader, &pad))
			{
				free (name);
				goto error;
			}
			lisrv_object_animate (self, name, channel, priority, 1);
			free (name);
		}
	}
	else if (!strcmp (type, "script"))
	{
		if (!liscr_data_read (self->script, serialize))
			goto error;
	}
	else
	{
		li_reader_set_offset (reader, end);
		free (type);
		return 1;
	}
	free (type);

	/* Check for read size. */
	if (li_reader_get_offset (serialize->reader) != end)
	{
		lisys_error_set (EINVAL, "block size mismatch");
		li_reader_set_offset (serialize->reader, end);
		return 0;
	}

	return 1;

error:
	li_reader_set_offset (serialize->reader, end);
	free (type);
	return 0;
}

/** @} */
/** @} */
