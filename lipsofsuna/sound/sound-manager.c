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
 * \addtogroup lisnd Sound 
 * @{
 * \addtogroup lisndManager Manager 
 * @{
 */

#ifndef LI_DISABLE_SOUND

#include "sound-manager.h"

/**
 * \brief Creates a new sample database.
 *
 * \param system Sound system.
 * \return New sound manager or NULL.
 */
lisndManager*
lisnd_manager_new (lisndSystem* system)
{
	lisndManager* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lisndManager));
	if (self == NULL)
		return NULL;
	self->system = system;
	self->samples = lialg_strdic_new ();
	if (self->samples == NULL)
	{
		lisnd_manager_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the sound manager.
 *
 * \param self Sound manager.
 */
void
lisnd_manager_free (lisndManager* self)
{
	lialgStrdicIter iter;

	if (self->samples != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->samples)
			lisnd_sample_free (iter.value);
		lialg_strdic_free (self->samples);
	}
	free (self);
}

/**
 * \brief Clears all the loaded samples.
 *
 * \param self Sound manager.
 */
void
lisnd_manager_clear (lisndManager* self)
{
	lialgStrdicIter iter;

	LI_FOREACH_STRDIC (iter, self->samples)
		lisnd_sample_free (iter.value);
	lialg_strdic_clear (self->samples);
}

/**
 * \brief Gets a sample by name.
 *
 * \param self Sound manager.
 * \param name Sample name.
 * \return Sample or NULL.
 */
lisndSample*
lisnd_manager_get_sample (lisndManager* self,
                          const char*   name)
{
	return lialg_strdic_find (self->samples, name);
}

/**
 * \brief Assigns a file to a sample name.
 *
 * \param self Sound manager.
 * \param name Sample name.
 * \param path Path to the sample.
 * \return Nonzero if the sample was loaded successfully.
 */
int
lisnd_manager_set_sample (lisndManager* self,
                          const char*   name,
                          const char*   path)
{
	lisndSample* sample;

	/* Only load once. */
	sample = lialg_strdic_find (self->samples, name);
	if (sample != NULL)
		return 1;

	/* Load the sample. */
	sample = lisnd_sample_new (self->system, path);
	if (sample == NULL)
		return 0;

	/* Store to database. */
	if (!lialg_strdic_insert (self->samples, name, sample))
	{
		lisnd_sample_free (sample);
		return 0;
	}

	return 1;
}

#endif

/** @} */
/** @} */

