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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <engine/lips-engine.h>
#include <script/lips-script.h>
#include <server/lips-server.h>

#define LIEXT_SCRIPT_SKILL "Lips.Skill"
#define LIEXT_SCRIPT_SKILLS "Lips.Skills"

typedef enum _liextSkillType liextSkillType;
typedef struct _liextSkill liextSkill;
typedef struct _liextSkills liextSkills;
typedef struct _liextModule liextModule;

struct _liextModule
{
	lialgPtrdic* dictionary;
	licalHandle calls[2];
	lisrvServer* server;
};

liextModule*
liext_module_new (lisrvServer* server);

void
liext_module_free (liextModule* self);

int
liext_module_insert_skills (liextModule* self,
                            liengObject* key,
                            liextSkills* value);

void
liext_module_remove_skills (liextModule* self,
                            liengObject* key,
                            liextSkills* value);

/*****************************************************************************/

void
liextSkillScript (liscrClass* self,
                  void*       data);

void
liextSkillsScript (liscrClass* self,
                   void*       data);

#endif

/** @} */
/** @} */
/** @} */
