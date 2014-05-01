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

#include "ext-module.h"

static void Parallel_update_physics_and_render (LIScrArgs* args)
{
	float secs;
	LIExtParallel* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PARALLEL);
	if (liscr_args_geti_float (args, 0, &secs))
		liext_parallel_update_physics_and_render (module, secs);
}

/*****************************************************************************/

void liext_script_parallel (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_PARALLEL, "parallel_update_physics_and_render", Parallel_update_physics_and_render);
}

/** @} */
/** @} */
