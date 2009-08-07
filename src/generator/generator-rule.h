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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup ligenRule Rule
 * @{
 */

#ifndef __GENERATOR_RULE_H__
#define __GENERATOR_RULE_H__

enum
{
	LIGEN_RULE_GENERATE = 0x00,
	LIGEN_RULE_REQUIRE = 0x01
};

typedef struct _ligenRulestroke ligenRulestroke;
struct _ligenRulestroke
{
	int pos[3];
	int brush;
	int flags;
};

typedef struct _ligenRule ligenRule;
struct _ligenRule
{
	int id;
	int flags;
	char* name;
	struct
	{
		int count;
		ligenRulestroke* array;
	} strokes;
};

ligenRule*
ligen_rule_new ();

void
ligen_rule_free (ligenRule* self);

int
ligen_rule_insert_stroke (ligenRule* self,
                          int        x,
                          int        y,
                          int        z,
                          int        flags,
                          int        brush);

void
ligen_rule_remove_stroke (ligenRule* self,
                          int        index);

int
ligen_rule_set_name (ligenRule*  self,
                     const char* value);

#endif

/** @} */
/** @} */
