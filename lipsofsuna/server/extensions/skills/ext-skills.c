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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvSkills Skills
 * @{
 */

#include <lipsofsuna/network.h>
#include "ext-module.h"
#include "ext-skills.h"

static int
private_send_clear (LIExtSkills* self);

static int
private_send_reset (LIExtSkills* self);

static int
private_send_skill (LIExtSkills* self,
                    LIExtSkill*  skill);

/*****************************************************************************/

LIExtSkills*
liext_skills_new (LIExtModule* module)
{
	LIExtSkills* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtSkills));
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
		return NULL;
	}

	return self;
}

void
liext_skills_free (LIExtSkills* self)
{
	LIAlgStrdicIter iter;

	/* No need to unreference the owner here because this is only called from
	 * the garbage collection function and the owner is collected already. */
	if (self->owner)
		lialg_ptrdic_remove (self->module->dictionary, self->owner);

	LI_FOREACH_STRDIC (iter, self->skills)
		liext_skill_free (iter.value);
	lialg_strdic_free (self->skills);
	lisys_free (self);
}

LIExtSkill*
liext_skills_find_skill (LIExtSkills* self,
                         const char*  name)
{
	return lialg_strdic_find (self->skills, name);
}

int
liext_skills_insert_skill (LIExtSkills*   self,
                           LIExtSkillType type,
                           const char*    name)
{
	LIExtSkill* skill;

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
liext_skills_update (LIExtSkills* self,
                     float        secs)
{
	float value;
	LIAlgStrdicIter iter;
	LIExtSkill* skill;

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
liext_skills_update_skill (LIExtSkills* self,
                           LIExtSkill*  skill)
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

LIEngObject*
liext_skills_get_owner (LIExtSkills* self)
{
	return self->owner;
}

int
liext_skills_set_owner (LIExtSkills* self,
                        LIEngObject* value)
{
	LIExtSkills* old;

	if (self->owner == value)
		return 1;

	/* Clear old client. */
	if (self->owner != NULL)
	{
		if (!private_send_clear (self))
			return 0;
	}

	/* Clear old skills. */
	if (value != NULL)
	{
		old = lialg_ptrdic_find (self->module->dictionary, value);
		if (old != NULL)
		{
			private_send_clear (old);
			liscr_data_unref (old->owner->script, old->script);
			liscr_data_unref (old->script, old->owner->script);
			lialg_ptrdic_remove (self->module->dictionary, value);
			old->owner = NULL;
		}
	}

	/* Set new owner. */
	if (self->owner != NULL)
	{
		liscr_data_unref (self->owner->script, self->script);
		liscr_data_unref (self->script, self->owner->script);
		lialg_ptrdic_remove (self->module->dictionary, self->owner);
	}
	if (value != NULL)
	{
		liscr_data_ref (value->script, self->script);
		liscr_data_ref (self->script, value->script);
		lialg_ptrdic_insert (self->module->dictionary, value, self);
	}

	/* Set new client. */
	self->owner = value;
	if (self->owner != NULL)
	{
		if (!private_send_reset (self))
			return 0;
	}

	return 1;
}

/*****************************************************************************/

static int
private_send_clear (LIExtSkills* self)
{
	LIArcWriter* writer;
	LIEngObject* object;
	LISerObserverIter iter;

	/* Create packet. */
	writer = liarc_writer_new_packet (LIEXT_SKILLS_PACKET_RESET);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint32 (writer, self->owner->id);

	/* Send to all observers. */
	LISER_FOREACH_OBSERVER (iter, self->owner, 1)
	{
		object = iter.object;
		liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	}

	liarc_writer_free (writer);

	return 1;
}

static int
private_send_reset (LIExtSkills* self)
{
	LIAlgStrdicIter iter0;
	LIArcWriter* writer;
	LIEngObject* object;
	LIExtSkill* skill;
	LISerObserverIter iter;

	/* Create public skill packet. */
	writer = liarc_writer_new_packet (LIEXT_SKILLS_PACKET_RESET);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint32 (writer, self->owner->id);
	LI_FOREACH_STRDIC (iter0, self->skills)
	{
		skill = iter0.value;
		if (skill->type != LIEXT_SKILL_TYPE_PUBLIC)
			continue;
		liarc_writer_append_string (writer, skill->name);
		liarc_writer_append_nul (writer);
		liarc_writer_append_float (writer, skill->value);
		liarc_writer_append_float (writer, skill->maximum);
	}

	/* Send to observers. */
	LISER_FOREACH_OBSERVER (iter, self->owner, 1)
	{
		object = iter.object;
		if (object != self->owner)
			liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	}

	if (LISER_OBJECT (self->owner)->client != NULL)
	{
		/* Create private skill packet. */
		LI_FOREACH_STRDIC (iter0, self->skills)
		{
			skill = iter0.value;
			if (skill->type != LIEXT_SKILL_TYPE_PRIVATE)
				continue;
			liarc_writer_append_string (writer, skill->name);
			liarc_writer_append_nul (writer);
			liarc_writer_append_float (writer, skill->value);
			liarc_writer_append_float (writer, skill->maximum);
		}

		/* Send to owner. */
		liser_client_send (LISER_OBJECT (self->owner)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

static int
private_send_skill (LIExtSkills* self,
                    LIExtSkill*  skill)
{
	LIArcWriter* writer;
	LIEngObject* object;
	LISerObserverIter iter;

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

	/* Send to observers. */
	if (skill->type == LIEXT_SKILL_TYPE_PUBLIC)
	{
		LISER_FOREACH_OBSERVER (iter, self->owner, 1)
		{
			object = iter.object;
			if (object != self->owner)
				liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		}
	}

	/* Send to owner. */
	if (LISER_OBJECT (self->owner)->client != NULL)
	{
		liser_client_send (LISER_OBJECT (self->owner)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
