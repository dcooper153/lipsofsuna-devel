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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndScene Scene
 * @{
 */

#ifndef __RENDER_SCENE_H__
#define __RENDER_SCENE_H__

#include "render-types.h"

struct _lirndScene
{
	int (*begin)(lirndSceneIter*, lirndScene*);
	int (*next)(lirndSceneIter*);
	void* data;
};

struct _lirndSceneIter
{
	lirndScene* scene;
	lirndObject* value;
	char data[256];
};

#define LIRND_FOREACH_SCENE(i, s) \
	for ((s)->begin (&(i), s) ; (i).value != NULL ; (s)->next (&(i)))

#endif

/** @} */
/** @} */
