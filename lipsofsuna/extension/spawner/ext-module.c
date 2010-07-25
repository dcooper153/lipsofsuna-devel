/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup LIExtSpawner Spawner    
 * @{
 */

#include "ext-module.h"
#include "ext-spawner.h"

LIMaiExtensionInfo liext_spawner_info =
{
	LIMAI_EXTENSION_VERSION, "Spawner",
	liext_spawners_new,
	liext_spawners_free
};

LIExtModule* liext_spawners_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Allocate dictionary. */
	self->dictionary = lialg_ptrdic_new ();
	if (self->dictionary == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Spawner", liext_script_spawner, self);

	return self;
}

void liext_spawners_free (
	LIExtModule* self)
{
	if (self->dictionary != NULL)
		lialg_ptrdic_free (self->dictionary);
	lisys_free (self);
}

LIExtSpawner* liext_spawners_find_spawner (
	LIExtModule* self,
	LIEngObject* owner)
{
	return lialg_ptrdic_find (self->dictionary, owner);
}

/** @} */
/** @} */
