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
 * \addtogroup LIExtNpc Npc      
 * @{
 */

#include "ext-module.h"
#include "ext-npc.h"

LIMaiExtensionInfo liext_npc_info =
{
	LIMAI_EXTENSION_VERSION, "Npc",
	liext_npcs_new,
	liext_npcs_free
};

LIExtModule* liext_npcs_new (
	LIMaiProgram* program)
{
	void* fun;
	LIExtModule* self;
	LIMaiExtension* ext;

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

	/* Check for voxel terrain. */
	ext = limai_program_find_extension (program, "voxel");
	if (ext != NULL)
	{
		fun = lisys_module_symbol (ext->module, "liext_module_get_voxels");
		if (fun != NULL)
			self->voxels = ((LIVoxManager* (*)(void*)) fun)(ext->object);
	}

	/* Allocate AI manager. */
	self->ai = liai_manager_new (program->callbacks, program->sectors, self->voxels);
	if (self->ai == NULL)
	{
		liext_npcs_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Npc", liext_script_npc, self);

	return self;
}

void liext_npcs_free (
	LIExtModule* self)
{
	if (self->dictionary != NULL)
		lialg_ptrdic_free (self->dictionary);
	if (self->ai != NULL)
		liai_manager_free (self->ai);
	lisys_free (self);
}

LIExtNpc* liext_npcs_find_npc (
	LIExtModule* self,
	LIEngObject* owner)
{
	return lialg_ptrdic_find (self->dictionary, owner);
}

/**
 * \brief Solves path to the requested point.
 *
 * \param self Module.
 * \param object Object.
 * \param target Target position vector.
 * \return New path or NULL if couldn't solve.
 */
LIAiPath* liext_npcs_solve_path (
	LIExtModule*       self,
	const LIEngObject* object,
	const LIMatVector* target)
{
	LIMatTransform transform;

	lieng_object_get_transform (object, &transform);
	transform.position.y += 0.5f;

	return liai_manager_solve_path (self->ai, &transform.position, target);
}

/** @} */
/** @} */
