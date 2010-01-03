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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSkills Skills
 * @{
 */

#ifndef __EXT_SKILLS_H__
#define __EXT_SKILLS_H__

#include "ext-module.h"

struct _LIExtSkill
{
	float value;
	float maximum;
	LIScrData* script;
};

struct _LIExtSkills
{
	LIAlgStrdic* skills;
	LIExtModule* module;
	LIScrData* script;
};

LIExtSkills*
liext_skills_new (LIExtModule* module);

void
liext_skills_free (LIExtSkills* self);

void
liext_skills_unref (LIExtSkills* self);

void
liext_skills_clear (LIExtSkills* self);

LIExtSkill*
liext_skills_find_skill (LIExtSkills* self,
                         const char*  name);

int
liext_skills_set_skill (LIExtSkills* self,
                        const char*  name,
                        float        value,
                        float        maximum);

#endif

/** @} */
/** @} */
/** @} */
