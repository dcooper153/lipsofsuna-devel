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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#ifndef __EXT_RULE_H__
#define __EXT_RULE_H__

#include <engine/lips-engine.h>
#include <math/lips-math.h>

typedef struct _liextRuleLine liextRuleLine;
struct _liextRuleLine
{
	liengModel* model;
	limatTransform transform;
/*
	struct
	{
		int id;
		char* name;
	} model;
*/
};

typedef struct _liextRule liextRule;
struct _liextRule
{
	float weight;
	struct
	{
		int count;
		liextRuleLine* array;
	} lines;
};

liextRule*
liext_rule_new ();

void
liext_rule_free (liextRule* self);

int
liext_rule_create_line (liextRule*            self,
                        const limatTransform* transform,
                        const liengModel*     model);

int
liext_rule_matches (const liextRule*   self,
                    int                line,
                    const liengObject* root,
                    const liengObject* object);

void
liext_rule_get_transform (const liextRule* self,
                          int              root,
                          int              line,
                          limatTransform*  transform);

#endif

/** @} */
/** @} */
/** @} */
