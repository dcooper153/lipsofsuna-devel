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

#include <network/lips-network.h>
#include "ext-module.h"
#include "ext-skills.h"

static int
private_send_clear (liextSkills* self);

static int
private_send_reset (liextSkills* self);

static int
private_send_skill (liextSkills* self,
                    liextSkill*  skill);

/*****************************************************************************/

liextSkills*
liext_skills_new (liextModule* module)
{
	liextSkills* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextSkills));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->server = module->server;
	self->skills = lialg_strdic_new ();
	if (self->skills == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate script. */
	self->script = liscr_data_new (module->server->script, self, LIEXT_SCRIPT_SKILLS, liext_skills_free);
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

	liext_skills_set_owner (self, NULL);
	LI_FOREACH_STRDIC (iter, self->skills)
		liext_skill_free (iter.value);
	lialg_strdic_free (self->skills);
	lisys_free (self);
}

liextSkill*
liext_skills_find_skill (liextSkills* self,
                         const char*  name)
{
	return lialg_strdic_find (self->skills, name);
}

int
liext_skills_insert_skill (liextSkills*   self,
                           liextSkillType type,
                           const char*    name)
{
	liextSkill* skill;

	/* Create skill. */
	skill = liext_skill_new (self, type, name);
	if (skill == NULL)
		return 0;
	if (!lialg_strdic_insert (self->skills, name, skill))
	{
		liext_skill_free (skill);
		return 0;
	}

	/* Inform clients. */
	if (self->owner != NULL)
	{
		if (!private_send_skill (self, skill))
			return 0;
	}

	return 1;
}

void
liext_skills_update (liextSkills* self,
                     float        secs)
{
	float value;
	lialgStrdicIter iter;
	liextSkill* skill;

	LI_FOREACH_STRDIC (iter, self->skills)
	{
		skill = iter.value;
		value = skill->value;
		if (skill->value > skill->maximum)
		{
			skill->value -= secs * skill->regen;
			if (skill->value < skill->maximum)
				skill->value = skill->maximum;
		}
		else
		{
			skill->value += secs * skill->regen;
			if (skill->value > skill->maximum)
				skill->value = skill->maximum;
		}
		if ((int) value != (int) skill->value)
			liext_skills_update_skill (self, skill);
	}
}

int
liext_skills_update_skill (liextSkills* self,
                           liextSkill*  skill)
{
	assert (skill == liext_skills_find_skill (self, skill->name));

	/* Inform clients. */
	if (self->owner != NULL)
	{
		if (!private_send_skill (self, skill))
			return 0;
	}

	return 1;
}

liengObject*
liext_skills_get_owner (liextSkills* self)
{
	return self->owner;
}

int
liext_skills_set_owner (liextSkills* self,
                        liengObject* value)
{
	if (self->owner == value)
		return 1;

	/* Inform clients. */
	if (self->owner != NULL)
	{
		if (!private_send_clear (self))
			return 0;
	}

	/* Set new owner. */
	if (self->owner != NULL)
		liext_module_remove_skills (self->module, self->owner, self);
	self->owner = value;
	if (self->owner != NULL)
		liext_module_insert_skills (self->module, self->owner, self);

	/* Inform clients. */
	if (self->owner != NULL)
	{
		if (!private_send_reset (self))
			return 0;
	}

	return 1;
}

/*****************************************************************************/

static int
private_send_clear (liextSkills* self)
{
	liarcWriter* writer;
	liengObject* object;
	lisrvObserverIter iter;

	/* Create packet. */
	writer = liarc_writer_new_packet (LIEXT_SKILLS_PACKET_RESET);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint32 (writer, self->owner->id);

	/* Send to all observers. */
	LISRV_FOREACH_OBSERVER (iter, self->owner, 1)
	{
		object = iter.object;
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	}

	liarc_writer_free (writer);

	return 1;
}

static int
private_send_reset (liextSkills* self)
{
	lialgStrdicIter iter0;
	liarcWriter* writer;
	liengObject* object;
	liextSkill* skill;
	lisrvObserverIter iter;

	/* Create packet. */
	writer = liarc_writer_new_packet (LIEXT_SKILLS_PACKET_RESET);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint32 (writer, self->owner->id);
	LI_FOREACH_STRDIC (iter0, self->skills)
	{
		skill = iter0.value;
		if (skill->type == LIEXT_SKILL_TYPE_INTERNAL)
			continue;
		liarc_writer_append_string (writer, skill->name);
		liarc_writer_append_nul (writer);
		liarc_writer_append_float (writer, skill->value);
		liarc_writer_append_float (writer, skill->maximum);
	}

	/* Send to all observers. */
	LISRV_FOREACH_OBSERVER (iter, self->owner, 1)
	{
		object = iter.object;
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	}

	liarc_writer_free (writer);

	return 1;
}

static int
private_send_skill (liextSkills* self,
                    liextSkill*  skill)
{
	liarcWriter* writer;
	liengObject* object;
	lisrvObserverIter iter;

	if (skill->type == LIEXT_SKILL_TYPE_INTERNAL)
		return 1;

	/* Create packet. */
	writer = liarc_writer_new_packet (LIEXT_SKILLS_PACKET_DIFF);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint32 (writer, self->owner->id);
	liarc_writer_append_string (writer, skill->name);
	liarc_writer_append_nul (writer);
	liarc_writer_append_float (writer, skill->value);
	liarc_writer_append_float (writer, skill->maximum);

	/* Send to all observers. */
	if (skill->type == LIEXT_SKILL_TYPE_PUBLIC)
	{
		LISRV_FOREACH_OBSERVER (iter, self->owner, 1)
		{
			object = iter.object;
			lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		}
	}
	else if (LISRV_OBJECT (self->owner)->client != NULL)
	{
		lisrv_client_send (LISRV_OBJECT (self->owner)->client, writer, GRAPPLE_RELIABLE);
	}

	liarc_writer_free (writer);

	return 1;
}

/** @} */
/** @} */
/** @} */
