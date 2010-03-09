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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup LIEngResources Resources
 * @{
 */

#ifndef __ENGINE_RESOURCES_H__
#define __ENGINE_RESOURCES_H__

#include <lipsofsuna/algorithm.h>
#include "engine-model.h"
#include "engine-types.h"

struct _LIEngResources
{
	LIEngEngine* engine;
	LIAlgStrdic* models;
};

LIEngResources*
lieng_resources_new (LIEngEngine* engine);

void
lieng_resources_free (LIEngResources* self);

void
lieng_resources_clear (LIEngResources* self);

LIEngModel*
lieng_resources_find_model (LIEngResources* self,
                            const char*     name);

#endif

/** @} */
/** @} */
