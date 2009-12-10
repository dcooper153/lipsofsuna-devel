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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvNpc Npc      
 * @{
 */

#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-npc.h"

lisrvExtensionInfo liextInfo =
{
	LISRV_EXTENSION_VERSION, "Npc",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (lisrvServer* server)
{
	void* fun;
	liextModule* self;
	lisrvExtension* ext;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->server = server;

	/* Check for voxel terrain. */
	ext = lisrv_server_find_extension (server, "voxel");
	if (ext != NULL)
	{
		fun = lisys_module_symbol (ext->module, "liext_module_get_voxels");
		if (fun != NULL)
			self->voxels = ((livoxManager* (*)(void*)) fun)(ext->object);
	}

	/* Allocate AI manager. */
	self->ai = liai_manager_new (self->voxels);
	if (self->ai == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register clases. */
	liscr_script_create_class (server->script, "Npc", liextNpcScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	if (self->ai != NULL)
		liai_manager_free (self->ai);
	lisys_free (self);
}

/**
 * \brief Solves path to the requested point.
 *
 * \param self Module.
 * \param object Object.
 * \param target Target position vector.
 * \return New path or NULL if couldn't solve.
 */
liaiPath*
liext_module_solve_path (liextModule*       self,
                         const liengObject* object,
                         const limatVector* target)
{
	limatTransform transform;

	lieng_object_get_transform (object, &transform);

	return liai_manager_solve_path (self->ai, &transform.position, target);
}

/** @} */
/** @} */
/** @} */
