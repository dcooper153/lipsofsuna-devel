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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <algorithm/lips-algorithm.h>

#define LIEXT_SCRIPT_SKILL "Lips.Skill"
#define LIEXT_SCRIPT_SKILLS "Lips.Skills"
#define LIEXT_SCRIPT_SKILL_WIDGET "Lips.SkillWidget"

typedef struct _liextModule liextModule;
typedef struct _liextSkill liextSkill;
typedef struct _liextSkills liextSkills;

struct _liextModule
{
	lialgU32dic* dictionary;
	licalHandle calls[2];
	licliClient* client;
};

liextModule*
liext_module_new (licliClient* client);

void
liext_module_free (liextModule* self);

liextSkills*
liext_module_find_skills (liextModule* self,
                          uint32_t     id);

/*****************************************************************************/

void
liextSkillsScript (liscrClass* self,
                   void*       data);

void
liextSkillWidgetScript (liscrClass* self,
                        void*       data);

#endif

/** @} */
/** @} */
/** @} */
