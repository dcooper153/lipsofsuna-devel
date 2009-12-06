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

#include "ext-skill.h"

liextSkill*
liext_skill_new (liextSkills*   skills,
                 liextSkillType type,
                 const char*    name)
{
	liextSkill* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextSkill));
	if (self == NULL)
		return NULL;
	self->skills = skills;
	self->type = type;
	self->name = listr_dup (name);
	if (self->name == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
liext_skill_free (liextSkill* self)
{
	lisys_free (self);
}

int
liext_skill_set_maximum (liextSkill* self,
                         float       value)
{
	if (self->maximum == value)
		return 1;
	self->maximum = value;
	liext_skills_update_skill (self->skills, self);

	return 1;
}

int
liext_skill_set_regen (liextSkill* self,
                       float       value)
{
	if (self->regen == value)
		return 1;
	self->regen = value;

	return 1;
}

int
liext_skill_set_value (liextSkill* self,
                       float       value)
{
	if (self->value == value)
		return 1;
	self->value = value;
	liext_skills_update_skill (self->skills, self);

	return 1;
}

/** @} */
/** @} */
/** @} */
