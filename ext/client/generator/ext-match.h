/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#ifndef __EXT_MATCH_H__
#define __EXT_MATCH_H__

#include <engine/lips-engine.h>
#include "ext-rule.h"

typedef struct _liextMatch liextMatch;
struct _liextMatch
{
	liextRule* rule;
	liengObject* root;
	int line;
	struct
	{
		int count;
		liengObject** array;
	} objects;
	struct
	{
		int count;
		float fraction;
	} matches;
};

liextMatch*
liext_match_new (liengObject* root,
                 liextRule*    rule,
                 int           line);

void
liext_match_free (liextMatch* self);

int
liext_match_find_object (const liextMatch*  self,
                         const liengObject* object);

int
liext_match_intersects_object (const liextMatch*  self,
                               const liengObject* object);

#endif

/** @} */
/** @} */
/** @} */
