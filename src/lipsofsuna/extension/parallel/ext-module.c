/* Lips of Suna
 * Copyright© 2007-2014 Lips of Suna development team.
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
 * \addtogroup LIExtParallel Parallel
 * @{
 */

#include "lipsofsuna/extension/physics/physics.h"
#include "lipsofsuna/render.h"
#include "ext-module.h"

#ifndef LI_DISABLE_GRAPHICS
typedef struct _LIExtParallelPhysicsParams LIExtParallelPhysicsParams;
struct _LIExtParallelPhysicsParams
{
	LIPhyPhysics* physics;
	float secs;
};

static void private_physics_main (LISysThread* thread, void* data);
#endif

/*****************************************************************************/

LIMaiExtensionInfo liext_parallel_info =
{
	LIMAI_EXTENSION_VERSION, "Parallel",
	liext_parallel_new,
	liext_parallel_free,
	liext_parallel_get_memstat
};

LIExtParallel* liext_parallel_new (
	LIMaiProgram* program)
{
	LIExtParallel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtParallel));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Register component. */
	if (!limai_program_insert_component (self->program, "parallel", self))
	{
		liext_parallel_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_PARALLEL, self);
	liext_script_parallel (program->script);

	return self;
}

void liext_parallel_free (
	LIExtParallel* self)
{
	/* Remove the components. */
	if (self->program != NULL)
		limai_program_remove_component (self->program, "parallel");

	lisys_free (self);
}

void liext_parallel_update_physics_and_render (
	LIExtParallel* self,
	float          secs)
{
#ifndef LI_DISABLE_GRAPHICS
	LIExtParallelPhysicsParams params;
	LIPhyPhysics* physics;
	LIRenRender* render;
	LISysThread* thread;

	/* Find the components. */
	physics = limai_program_find_component (self->program, "physics");
	render = limai_program_find_component (self->program, "render");

	if (physics != NULL && render != NULL)
	{
		params.physics = physics;
		params.secs = secs;
		thread = lisys_thread_new (private_physics_main, &params);
		liren_render_render (render);
		if (thread != NULL)
			lisys_thread_free (thread);
	}
	else if (render != NULL)
		liren_render_render (render);
	else if (physics != NULL)
		liphy_physics_update (physics, secs);
#else
	LIPhyPhysics* physics;

	physics = limai_program_find_component (self->module->program, "physics");
	if (physics != NULL)
		liphy_physics_update (physics, secs);
#endif
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_parallel_get_memstat (
	LIExtParallel* self,
	LIMaiMemstat*  stat)
{
/* TODO: memstat */
}

/*****************************************************************************/

#ifndef LI_DISABLE_GRAPHICS
static void private_physics_main (LISysThread* thread, void* data)
{
	LIExtParallelPhysicsParams* params = data;

	liphy_physics_update (params->physics, params->secs);
}
#endif

/** @} */
/** @} */
