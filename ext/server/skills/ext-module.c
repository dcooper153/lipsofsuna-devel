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
#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-skills.h"

static int
private_tick (liextModule* self,
              float        secs);

static int
private_vision_show (liextModule* self,
                     liengObject* object,
                     liengObject* observer);

/*****************************************************************************/

lisrvExtensionInfo liextInfo =
{
	LISRV_EXTENSION_VERSION, "Skills",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (lisrvServer* server)
{
	liextModule* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->server = server;
	self->dictionary = lialg_ptrdic_new ();
	if (self->dictionary == NULL)
	{
		free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lieng_engine_insert_call (server->engine, LISRV_CALLBACK_TICK, 0,
	     	private_tick, self, self->calls + 0) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_VISION_SHOW, 0,
	     	private_vision_show, self, self->calls + 1))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_insert_class (server->script, "Skill", liextSkillScript, self);
	liscr_script_insert_class (server->script, "Skills", liextSkillsScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* FIXME: Remove the class here. */
	lialg_ptrdic_free (self->dictionary);
	lieng_engine_remove_calls (self->server->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));
	free (self);
}

int
liext_module_insert_skills (liextModule* self,
                            liengObject* key,
                            liextSkills* value)
{
	if (lialg_ptrdic_insert (self->dictionary, key, value))
		return 1;
	return 0;
}

void
liext_module_remove_skills (liextModule* self,
                            liengObject* key,
                            liextSkills* value)
{
	lialg_ptrdic_remove (self->dictionary, key);
}

/*****************************************************************************/

static int
private_tick (liextModule* self,
              float        secs)
{
	lialgPtrdicIter iter;
	liextSkills* skills;

	LI_FOREACH_PTRDIC (iter, self->dictionary)
	{
		skills = iter.value;
		liext_skills_update (skills, secs);
	}

	return 1;
}

static int
private_vision_show (liextModule* self,
                     liengObject* object,
                     liengObject* target)
{
	int perm;
	lialgStrdicIter iter;
	liarcWriter* writer;
	liextSkill* skill;
	liextSkills* skills;

	/* Get skills data. */
	skills = lialg_ptrdic_find (self->dictionary, target);
	if (skills == NULL)
		return 1;
	perm = (target == object);

	/* Create reset packet. */
	writer = liarc_writer_new_packet (LIEXT_SKILLS_PACKET_RESET);
	if (writer == NULL)
		return 1;
	liarc_writer_append_uint32 (writer, target->id);
	LI_FOREACH_STRDIC (iter, skills->skills)
	{
		skill = iter.value;
		if (skill->type == LIEXT_SKILL_TYPE_INTERNAL)
			continue;
		if (!perm && skill->type == LIEXT_SKILL_TYPE_PRIVATE)
			continue;
		liarc_writer_append_string (writer, skill->name);
		liarc_writer_append_nul (writer);
		liarc_writer_append_float (writer, skill->value);
		liarc_writer_append_float (writer, skill->maximum);
	}

	/* Send to the client. */
	lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	liarc_writer_free (writer);

	return 1;
}

/** @} */
/** @} */
/** @} */
