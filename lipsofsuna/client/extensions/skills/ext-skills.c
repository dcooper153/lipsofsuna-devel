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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSkills Skills
 * @{
 */

#include <client/lips-client.h>
#include "ext-skills.h"

liextSkills*
liext_skills_new (liextModule* module)
{
	liextSkills* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextSkills));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->skills = lialg_strdic_new ();
	if (self->skills == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate script. */
	self->script = liscr_data_new (module->module->script, self, LIEXT_SCRIPT_SKILLS, liext_skills_free);
	if (self == NULL)
	{
		lialg_strdic_free (self->skills);
		lisys_free (self);
		return 0;
	}

	return self;
}

void
liext_skills_free (liextSkills* self)
{
	lialgStrdicIter iter;

	LI_FOREACH_STRDIC (iter, self->skills)
		lisys_free (iter.value);
	lialg_strdic_free (self->skills);
	lisys_free (self);
}

void
liext_skills_unref (liextSkills* self)
{
	liscr_data_unref (self->script, NULL);
}

void
liext_skills_clear (liextSkills* self)
{
	lialgStrdicIter iter;

	LI_FOREACH_STRDIC (iter, self->skills)
		lisys_free (iter.value);
	lialg_strdic_clear (self->skills);
}

liextSkill*
liext_skills_find_skill (liextSkills* self,
                         const char*  name)
{
	return lialg_strdic_find (self->skills, name);
}

int
liext_skills_set_skill (liextSkills* self,
                        const char*  name,
                        float        value,
                        float        maximum)
{
	liextSkill* skill;

	skill = lialg_strdic_find (self->skills, name);
	if (skill == NULL)
	{
		skill = lisys_calloc (1, sizeof (liextSkill));
		if (skill == NULL)
			return 0;
		if (!lialg_strdic_insert (self->skills, name, skill))
		{
			lisys_free (skill);
			return 0;
		}
	}
	skill->value = value;
	skill->maximum = maximum;

	return 1;
}

/** @} */
/** @} */
/** @} */
