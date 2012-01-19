/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtHeightmap Heightmap
 * @{
 */

#include "module.h"

LIMaiExtensionInfo liext_heightmap_info =
{
	LIMAI_EXTENSION_VERSION, "Heightmap",
	liext_heightmap_module_new,
	liext_heightmap_module_free
};

LIExtHeightmapModule* liext_heightmap_module_new (
	LIMaiProgram* program)
{
	LIExtHeightmapModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtHeightmapModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Allocate the dictionary. */
	self->heightmaps = lialg_u32dic_new ();
	if (self->heightmaps == NULL)
	{
		liext_heightmap_module_free (self);
		return NULL;
	}

	/* Register the component. */
	if (!limai_program_insert_component (self->program, "heightmap", self))
	{
		liext_heightmap_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_HEIGHTMAP, self);
	liext_script_heightmap (program->script);

	return self;
}

void liext_heightmap_module_free (
	LIExtHeightmapModule* self)
{
	/* Heightmaps should be garbage collected. */
	lisys_assert (self->heightmaps->size == 0);

	/* Render and physics modules should be freed before us. */
	lisys_assert (self->physics_hooks == NULL);
	lisys_assert (self->render_hooks == NULL);

	/* Remove the component. */
	limai_program_remove_component (self->program, "heightmap");

	/* Free self. */
	if (self->heightmaps != NULL)
		lialg_u32dic_free (self->heightmaps);
	lisys_free (self);
}

/** @} */
/** @} */
