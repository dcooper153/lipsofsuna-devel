/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIExtObjectRender ObjectRender
 * @{
 */

#include "ext-module.h"

LIMaiExtensionInfo liext_object_render_info =
{
	LIMAI_EXTENSION_VERSION, "ObjectRender",
	liext_object_render_new,
	liext_object_render_free,
	liext_object_render_get_memstat
};

LIExtModule* liext_object_render_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Allocate the object dictionary. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		liext_object_render_free (self);
		return NULL;
	}

	/* Find the client compoent. */
	self->render = limai_program_find_component (program, "render");
	if (self->render == NULL)
	{
		liext_object_render_free (self);
		return NULL;
	}

	/* Make sure that the required extensions are loaded. */
	if (!limai_program_insert_extension (program, "render"))
	{
		liext_object_render_free (self);
		return NULL;
	}

	/* Extend classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_RENDER_OBJECT, self);
	liext_script_render_object (program->script);

	return self;
}

void liext_object_render_free (
	LIExtModule* self)
{
	if (self->objects != NULL)
	{
		lisys_assert (self->objects->size == 0);
		lialg_u32dic_free (self->objects);
	}
	lisys_free (self);
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_object_render_get_memstat (
	LIExtModule*  self,
	LIMaiMemstat* stat)
{
/* TODO: memstat */
}

/** @} */
/** @} */
