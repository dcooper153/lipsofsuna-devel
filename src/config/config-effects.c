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
 * \addtogroup licfg Config
 * @{
 * \addtogroup licfgEffects Effects
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "config-effects.h"

/**
 * \brief Creates a new effect list.
 *
 * \param dir Data directory root or NULL if an empty list should be created.
 * \return New effect list or NULL.
 */
licfgEffects*
licfg_effects_new (const char* dir)
{
	uint8_t id = 0;
	char* name;
	char* path;
	char* sound;
	liReader* reader = NULL;
	licfgEffect* effect;
	licfgEffects* self;

	/* Allocate self. */
	self = calloc (1, sizeof (licfgEffects));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->bystr = lialg_strdic_new ();
	self->byid = lialg_u32dic_new ();
	if (self->bystr == NULL || self->byid == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}

	/* Check if loading is required. */
	if (dir == NULL)
		return self;

	/* Open the file. */
	path = lisys_path_concat (dir, "config", "effects.def", NULL);
	if (path == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}
	reader = li_reader_new_from_file (path);
	free (path);
	if (reader == NULL)
		goto error;

	/* Read effects. */
	while (1)
	{
		/* Check for syntax. */
		li_reader_skip_chars (reader, " \t\n");
		if (li_reader_check_end (reader))
			break;
		if (id == 255)
		{
			lisys_error_set (EINVAL, "effect number out of bounds");
			goto error;
		}

		/* Parse an effect. */
		name = NULL;
		sound = NULL;
		if (!li_reader_get_text (reader, " \t\n", &name) ||
		    !li_reader_get_text (reader, " \t\n", &sound))
		{
			free (name);
			free (sound);
			goto error;
		}

		/* Allocate the effect. */
		effect = calloc (1, sizeof (licfgEffect));
		if (effect == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			free (name);
			goto error;
		}
		effect->id = id++;
		effect->name = name;
		if (strcmp (sound, "none"))
			effect->sound = sound;
		else
			free (sound);

		/* Store the effect. */
		if (!lialg_strdic_insert (self->bystr, effect->name, effect))
		{
			free (effect->name);
			free (effect->sound);
			free (effect);
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		if (!lialg_u32dic_insert (self->byid, effect->id, effect))
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
	}

	li_reader_free (reader);
	return self;

error:
	if (reader != NULL)
		li_reader_free (reader);
	licfg_effects_free (self);
	return NULL;
}

/**
 * \brief Frees the effect configuration.
 *
 * \param self Effect configuration.
 */
void
licfg_effects_free (licfgEffects* self)
{
	lialgStrdicIter iter;
	licfgEffect* effect;

	if (self->bystr != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->bystr)
		{
			effect = iter.value;
			free (effect->name);
			free (effect->sound);
			free (effect);
		}
		lialg_strdic_free (self->bystr);
	}
	if (self->byid != NULL)
		lialg_u32dic_free (self->byid);
	free (self);
}

/**
 * \brief Finds an effect by code.
 *
 * \param self Effect configuration.
 * \param id Effect code.
 * \return Effect or NULL.
 */
licfgEffect*
licfg_effects_get_effect_by_code (licfgEffects* self,
                                  int           id)
{
	return lialg_u32dic_find (self->byid, id);
}

/**
 * \brief Finds an effect by name.
 *
 * \param self Effect configuration.
 * \param name Effect name.
 * \return Effect or NULL.
 */
licfgEffect*
licfg_effects_get_effect_by_name (licfgEffects* self,
                                  const char*   name)
{
	return lialg_strdic_find (self->bystr, name);
}

/**
 * \brief Gets the number of stored effects.
 *
 * \param self Effect configuration.
 * \return Effect count.
 */
int
licfg_effects_get_count (licfgEffects* self)
{
	return self->bystr->size;
}

/** @} */
/** @} */
