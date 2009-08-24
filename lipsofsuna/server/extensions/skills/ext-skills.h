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

#ifndef __EXT_SKILLS_H__
#define __EXT_SKILLS_H__

#include <algorithm/lips-algorithm.h>
#include <server/lips-server.h>
#include <system/lips-system.h>
#include "ext-module.h"
#include "ext-skill.h"

struct _liextSkills
{
	lialgStrdic* skills;
	liextModule* module;
	lisrvServer* server;
	liscrData* data;
	liengObject* owner;
};

liextSkills*
liext_skills_new (liextModule* module);

void
liext_skills_free (liextSkills* self);

liextSkill*
liext_skills_find_skill (liextSkills* self,
                         const char*  name);

int
liext_skills_insert_skill (liextSkills*   self,
                           liextSkillType type,
                           const char*    name);

void
liext_skills_update (liextSkills* self,
                     float        secs);

int
liext_skills_update_skill (liextSkills* self,
                           liextSkill*  skill);

liengObject*
liext_skills_get_owner (liextSkills* self);

int
liext_skills_set_owner (liextSkills* self,
                        liengObject* value);

#endif

/** @} */
/** @} */
/** @} */
