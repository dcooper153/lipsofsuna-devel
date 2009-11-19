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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextsrvSound Sound
 * @{
 */

#include "ext-module.h"

#ifndef LI_DISABLE_SOUND
static int
private_packet (liextModule* self,
                int          type,
                liarcReader* reader);

static int
private_tick (liextModule* self,
              float        secs);
#endif

/*****************************************************************************/

lisrvExtensionInfo liextInfo =
{
	LISRV_EXTENSION_VERSION, "Sound",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliModule* module)
{
	liextModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;

#ifndef LI_DISABLE_SOUND
	/* Allocate objects. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Initialize sound. */
	self->system = lisnd_system_new ();
	if (self->system != NULL)
	{
		self->sound = lisnd_manager_new (self->system);
		self->music = lisnd_source_new (self->system);
	}
	else
		printf ("WARNING: cannot initialize sound\n");

	/* Register callbacks. */
	if (!lieng_engine_insert_call (module->engine, LICLI_CALLBACK_PACKET, 1,
	     	private_packet, self, self->calls + 0) ||
	    !lieng_engine_insert_call (module->engine, LICLI_CALLBACK_TICK, 1,
	     	private_tick, self, self->calls + 1))
	{
		liext_module_free (self);
		return NULL;
	}
#endif

	/* Register classes. */
	liscr_script_create_class (module->script, "Sound", liextSoundScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
#ifndef LI_DISABLE_SOUND
	lialgU32dicIter iter;

	/* Remove callbacks. */
	lieng_engine_remove_calls (self->module->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));

	/* Free objects. */
	if (self->objects != NULL)
	{
		LI_FOREACH_U32DIC (iter, self->objects)
			liext_object_free (iter.value);
		lialg_u32dic_free (self->objects);
	}

	/* Disable sound. */
	if (self->music != NULL)
		lisnd_source_free (self->music);
	if (self->sound != NULL)
		lisnd_manager_free (self->sound);
	if (self->sound != NULL)
		lisnd_system_free (self->system);
#endif

	/* FIXME: Remove the class here. */
	lisys_free (self);
}

#ifndef LI_DISABLE_SOUND
/**
 * \brief Finds a sound sample by effect number.
 *
 * \param self Module.
 * \param id Effect number.
 * \return Sample owned by the module or NULL.
 */
lisndSample*
liext_module_find_sample_by_id (liextModule* self,
                                int          id)
{
	liengSample* sample;

	/* Find sample. */
	sample = lieng_resources_find_sample_by_code (self->module->engine->resources, id);
	if (sample == NULL || sample->invalid)
		return NULL;
	if (sample->data != NULL)
		return sample->data;

	/* Load sample. */
	if (!lisnd_manager_set_sample (self->sound, sample->name, sample->path))
	{
		lisys_error_report ();
		sample->invalid = 1;
		return NULL;
	}
	sample->data = lisnd_manager_get_sample (self->sound, sample->name);

	return sample->data;
}

/**
 * \brief Finds a sound sample by name.
 *
 * \param self Module.
 * \param name Name of the sample.
 * \return Sample owned by the module or NULL.
 */
lisndSample*
liext_module_find_sample_by_name (liextModule* self,
                                  const char*  name)
{
	liengSample* sample;

	/* Find sample. */
	sample = lieng_resources_find_sample_by_name (self->module->engine->resources, name);
	if (sample == NULL || sample->invalid)
		return NULL;
	if (sample->data != NULL)
		return sample->data;

	/* Load sample. */
	if (!lisnd_manager_set_sample (self->sound, sample->name, sample->path))
	{
		lisys_error_report ();
		sample->invalid = 1;
		return NULL;
	}
	sample->data = lisnd_manager_get_sample (self->sound, sample->name);

	return sample->data;
}

int
liext_module_set_effect (liextModule* self,
                         uint32_t     object,
                         uint32_t     effect,
                         int          flags)
{
	int create;
	liengObject* engobj;
	liextObject* extobj;
	limatTransform transform;
	limatVector vector;
	lisndSample* sample;
	lisndSource* source;

	/* Find sample. */
	if (self->sound == NULL)
		return 1;
	sample = liext_module_find_sample_by_id (self, effect);
	if (sample == NULL)
		return 0;

	/* Find engine object. */
	create = 0;
	engobj = lieng_engine_find_object (self->module->engine, object);
	if (engobj == NULL)
		return 0;

	/* Find or create sound object. */
	extobj = lialg_u32dic_find (self->objects, object);
	if (extobj == NULL)
	{
		create = 1;
		extobj = liext_object_new ();
		if (extobj == NULL)
			return 0;
		if (!lialg_u32dic_insert (self->objects, object, extobj))
		{
			liext_object_free (extobj);
			return 0;
		}
	}

	/* Allocate new source. */
	source = lisnd_source_new_with_sample (self->system, sample);
	if (source == NULL)
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_object_free (extobj);
		}
		return 0;
	}
	if (!lialg_list_prepend (&extobj->sounds, source))
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_object_free (extobj);
		}
		lisnd_source_free (source);
		return 0;
	}

	/* Set properties. */
	lieng_object_get_transform (engobj, &transform);
	lieng_object_get_velocity (engobj, &vector);
	lisnd_source_set_position (source, &transform.position);
	lisnd_source_set_velocity (source, &vector);
	if (flags & LI_EFFECT_REPEAT)
		lisnd_source_set_looping (source, 1);
	lisnd_source_set_playing (source, 1);

	return 1;
}

int
liext_module_set_music (liextModule* self,
                        const char*  value)
{
	lisndSample* sample;

	/* Find sample. */
	if (self->sound == NULL)
		return 1;
	sample = liext_module_find_sample_by_name (self, value);
	if (sample == NULL)
		return 0;

	/* Set music track. */
	/* FIXME: Doesn't work if already playing. */
	lisnd_source_queue_sample (self->music, sample);
	lisnd_source_set_looping (self->music, 1);
	lisnd_source_set_playing (self->music, 1);

	return 1;
}
#endif

/*****************************************************************************/

#ifndef LI_DISABLE_SOUND
liextObject*
liext_object_new ()
{
	return lisys_calloc (1, sizeof (liextObject));
}

void
liext_object_free (liextObject* self)
{
	lialgList* ptr;

	for (ptr = self->sounds ; ptr != NULL ; ptr = ptr->next)
		lisnd_source_free (ptr->data);
	lialg_list_free (self->sounds);
	lisys_free (self);
}
#endif

/*****************************************************************************/

#ifndef LI_DISABLE_SOUND
static int
private_packet (liextModule* self,
                int          type,
                liarcReader* reader)
{
	uint32_t id;
	uint16_t effect;
	uint16_t flags;

	reader->pos = 1;
	if (type == LINET_SERVER_PACKET_OBJECT_EFFECT)
	{
		if (!liarc_reader_get_uint32 (reader, &id) ||
			!liarc_reader_get_uint16 (reader, &effect) ||
			!liarc_reader_get_uint16 (reader, &flags) ||
			!liarc_reader_check_end (reader))
			return 1;
		liext_module_set_effect (self, id, effect, flags);
	}

	return 1;
}

static int
private_tick (liextModule* self,
              float        secs)
{
	lialgU32dicIter iter;
	lialgList* ptr;
	lialgList* next;
	liengObject* engobj;
	liextObject* extobj;
	limatTransform transform;
	limatVector direction;
	limatVector velocity;
	limatVector up;
	limatVector vector;
	lisndSource* source;

	/* Update listener position. */
	engobj = licli_module_get_player (self->module);
	if (engobj != NULL && self->module->network != NULL)
	{
		lieng_object_get_transform (engobj, &transform);
		lieng_object_get_velocity (engobj, &velocity);
		direction = limat_quaternion_get_basis (transform.rotation, 2);
		up = limat_quaternion_get_basis (transform.rotation, 1);
		lisnd_system_set_listener (self->system, &transform.position, &velocity, &direction, &up);
	}

	/* Update sound effects. */
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		extobj = iter.value;
		engobj = lieng_engine_find_object (self->module->engine, iter.key);
		if (engobj != NULL)
		{
			for (ptr = extobj->sounds ; ptr != NULL ; ptr = next)
			{
				next = ptr->next;
				source = ptr->data;
				lieng_object_get_transform (engobj, &transform);
				lieng_object_get_velocity (engobj, &vector);
				lisnd_source_set_position (source, &transform.position);
				lisnd_source_set_velocity (source, &vector);
				if (!lisnd_source_update (source))
				{
					lisnd_source_free (source);
					lialg_list_remove (&extobj->sounds, ptr);
				}
			}
		}
		if (engobj == NULL || extobj->sounds == NULL)
		{
			lialg_u32dic_remove (self->objects, iter.key);
			liext_object_free (extobj);
		}
	}

	return 1;
}
#endif

/** @} */
/** @} */
/** @} */
