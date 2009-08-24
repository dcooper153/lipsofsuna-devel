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
 * \addtogroup liextsrvSkills Skills
 * @{
 */

#ifndef __EXT_SKILL_H__
#define __EXT_SKILL_H__

#include <server/lips-server.h>
#include <system/lips-system.h>
#include "ext-module.h"
#include "ext-skills.h"

enum _liextSkillType
{
	LIEXT_SKILL_TYPE_INTERNAL,
	LIEXT_SKILL_TYPE_PUBLIC,
	LIEXT_SKILL_TYPE_PRIVATE,
	LIEXT_SKILL_TYPE_MAX
};

struct _liextSkill
{
	char* name;
	float value;
	float maximum;
	float regen;
	liextSkills* skills;
	liextSkillType type;
	liscrData* script;
};

liextSkill*
liext_skill_new (liextSkills*   skills,
                 liextSkillType type,
                 const char*    name);

void
liext_skill_free (liextSkill* self);

int
liext_skill_set_maximum (liextSkill* self,
                         float       value);

int
liext_skill_set_regen (liextSkill* self,
                       float       value);

int
liext_skill_set_value (liextSkill* self,
                       float       value);

#endif

/** @} */
/** @} */
/** @} */
