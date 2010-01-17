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

#ifndef __EXT_WIDGET_H__
#define __EXT_WIDGET_H__

#include <lipsofsuna/widget.h>
#include "ext-module.h"

#define LIEXT_SKILL_WIDGET(o) ((LIExtSkillWidget*)(o))

typedef struct _LIExtSkillWidget LIExtSkillWidget;
struct _LIExtSkillWidget
{
	LIWdgGroup base;
	char* skill;
	uint32_t object;
	LIExtModule* module;
	LIWdgWidget* value;
};

LIWdgWidget*
liext_skill_widget_new (LIExtModule* module);

int
liext_skill_widget_set_skill (LIExtSkillWidget* self,
                              uint32_t          object,
                              const char*       name);

#endif

/** @} */
/** @} */
/** @} */
