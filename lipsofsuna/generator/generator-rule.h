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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup LIGenRule Rule
 * @{
 */

#ifndef __GENERATOR_RULE_H__
#define __GENERATOR_RULE_H__

enum
{
	LIGEN_RULE_GENERATE = 0x00,
	LIGEN_RULE_REQUIRE = 0x01
};

typedef struct _LIGenRulestroke LIGenRulestroke;
struct _LIGenRulestroke
{
	int pos[3];
	int brush;
	int flags;
};

typedef struct _LIGenRule LIGenRule;
struct _LIGenRule
{
	int id;
	int flags;
	char* name;
	struct
	{
		int count;
		LIGenRulestroke* array;
	} strokes;
};

LIGenRule*
ligen_rule_new ();

LIGenRule*
ligen_rule_new_copy (LIGenRule* rule);

void
ligen_rule_free (LIGenRule* self);

int
ligen_rule_insert_stroke (LIGenRule* self,
                          int        x,
                          int        y,
                          int        z,
                          int        flags,
                          int        brush);

void
ligen_rule_remove_stroke (LIGenRule* self,
                          int        index);

int
ligen_rule_set_name (LIGenRule*  self,
                     const char* value);

#endif

/** @} */
/** @} */
