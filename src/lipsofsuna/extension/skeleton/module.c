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
 * \addtogroup LIExtSkeleton Skeleton
 * @{
 */

#include "module.h"

LIMaiExtensionInfo liext_skeleton_info =
{
	LIMAI_EXTENSION_VERSION, "Skeleton",
	liext_skeleton_module_new,
	liext_skeleton_module_free,
	liext_skeleton_module_get_memstat
};

/**
 * \brief Initializes the module.
 * \param program Program.
 * \return Module on success. NULL otherwise.
 */
LIExtSkeletonModule* liext_skeleton_module_new (
	LIMaiProgram* program)
{
	LIExtSkeletonModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtSkeletonModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_SKELETON, self);
	liext_script_skeleton (program->script);

	return self;
}

/**
 * \brief Frees the module.
 * \param self Module.
 */
void liext_skeleton_module_free (
	LIExtSkeletonModule* self)
{
	lisys_free (self);
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_skeleton_module_get_memstat (
	LIExtSkeletonModule* self,
	LIMaiMemstat*        stat)
{
/* TODO: memstat */
}

/** @} */
/** @} */
