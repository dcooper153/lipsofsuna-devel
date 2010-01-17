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
 * \addtogroup LIExtSkills Skills
 * @{
 */

#ifndef __EXT_SKILL_H__
#define __EXT_SKILL_H__

#include <lipsofsuna/server.h>
#include <lipsofsuna/system.h>
#include "ext-module.h"
#include "ext-skills.h"

enum _LIExtSkillType
{
	LIEXT_SKILL_TYPE_INTERNAL,
	LIEXT_SKILL_TYPE_PUBLIC,
	LIEXT_SKILL_TYPE_PRIVATE,
	LIEXT_SKILL_TYPE_MAX
};

struct _LIExtSkill
{
	char* name;
	float value;
	float maximum;
	float regen;
	LIExtSkills* skills;
	LIExtSkillType type;
};

LIExtSkill*
liext_skill_new (LIExtSkills*   skills,
                 LIExtSkillType type,
                 const char*    name);

void
liext_skill_free (LIExtSkill* self);

int
liext_skill_set_maximum (LIExtSkill* self,
                         float       value);

int
liext_skill_set_regen (LIExtSkill* self,
                       float       value);

int
liext_skill_set_value (LIExtSkill* self,
                       float       value);

#endif

/** @} */
/** @} */
