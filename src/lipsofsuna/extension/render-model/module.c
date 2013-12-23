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
 * \addtogroup LIExtRenderModel RenderModel
 * @{
 */

#include "module.h"

LIMaiExtensionInfo liext_render_model_info =
{
	LIMAI_EXTENSION_VERSION, "RenderModel",
	liext_render_model_module_new,
	liext_render_model_module_free,
	liext_render_model_module_get_memstat
};

LIExtRenderModelModule* liext_render_model_module_new (
	LIMaiProgram* program)
{
	LIExtRenderModelModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtRenderModelModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Allocate the model dictionary. */
	self->models = lialg_u32dic_new ();
	if (self->models == NULL)
	{
		liext_render_model_module_free (self);
		return NULL;
	}

	/* Find the client compoent. */
	self->render = limai_program_find_component (program, "render");
	if (self->render == NULL)
	{
		liext_render_model_module_free (self);
		return NULL;
	}

	/* Make sure that the required extensions are loaded. */
	if (!limai_program_insert_extension (program, "render"))
	{
		liext_render_model_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_RENDER_MODEL, self);
	liext_script_render_model (program->script);

	return self;
}

void liext_render_model_module_free (
	LIExtRenderModelModule* self)
{
	if (self->models != NULL)
	{
		lisys_assert (self->models->size == 0);
		lialg_u32dic_free (self->models);
	}
	lisys_free (self);
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_render_model_module_get_memstat (
	LIExtRenderModelModule* self,
	LIMaiMemstat*           stat)
{
	LIAlgU32dicIter i;

	LIALG_U32DIC_FOREACH (i, self->program->models->models)
	{
		limai_memstat_add_object (stat, "RenderModel", "RenderModel",
			liext_render_model_get_memory (i.value));
	}
}

/** @} */
/** @} */
