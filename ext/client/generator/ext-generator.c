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
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#include "ext-generator.h"
liextGenerator*
liext_generator_new (licliModule* module)
{
	liextGenerator* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liextGenerator));
	if (self == NULL)
		return self;
	self->module = module;

	/* Allocate preview scene. */
	self->scene = lirnd_scene_new (module->engine->render);
	if (self->scene == NULL)
	{
		free (self);
		return NULL;
	}

	/* Allocate generator. */
	self->generator = ligen_generator_new_full (module->paths->root,
		module->name, self->scene, module->engine->renderapi);
	if (self->generator == NULL)
	{
		lirnd_scene_free (self->scene);
		free (self);
		return NULL;
	}
	ligen_generator_set_fill (self->generator, 0);

	return self;
}

void
liext_generator_free (liextGenerator* self)
{
	if (self->generator != NULL)
		ligen_generator_free (self->generator);
	if (self->scene != NULL)
		lirnd_scene_free (self->scene);
	free (self);
}

/**
 * \brief Loads the generator rules.
 *
 * If the `path' is NULL, the rules are loaded from the default generator file
 * at `config/generator.dat'. Otherwise, the file specified by `path' is used.
 *
 * \param self Generator.
 * \param path Path or NULL.
 * \return Nonzero on success.
 */
int
liext_generator_load (liextGenerator* self,
                      const char*     path)
{
#warning Reloading generator data not implemented.
	return 1;
}

/**
 * \brief Saves the generator rules.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
liext_generator_save (liextGenerator* self)
{
	return ligen_generator_write_brushes (self->generator);
}

/** @} */
/** @} */
/** @} */
