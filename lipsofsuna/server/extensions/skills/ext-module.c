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
#include <lipsofsuna/server.h>
#include "ext-module.h"
#include "ext-skills.h"

static int
private_object_client (LIExtModule* self,
                       LIEngObject* object);

static int
private_tick (LIExtModule* self,
              float        secs);

static int
private_vision_show (LIExtModule* self,
                     LIEngObject* object,
                     LIEngObject* observer);

/*****************************************************************************/

LISerExtensionInfo liextInfo =
{
	LISER_EXTENSION_VERSION, "Skills",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LISerServer* server)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->server = server;

	/* Allocate dictionary. */
	self->dictionary = lialg_ptrdic_new ();
	if (self->dictionary == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (server->callbacks, server->engine, "object-client", 0, private_object_client, self, self->calls + 0) ||
	    !lical_callbacks_insert (server->callbacks, server->engine, "tick", 0, private_tick, self, self->calls + 1) ||
	    !lical_callbacks_insert (server->callbacks, server->engine, "vision-show", 0, private_vision_show, self, self->calls + 2))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (server->script, "Skills", liext_script_skills, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	if (self->dictionary != NULL)
		lialg_ptrdic_free (self->dictionary);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

LIExtSkills*
liext_module_find_skills (LIExtModule* self,
                          LIEngObject* owner)
{
	return lialg_ptrdic_find (self->dictionary, owner);
}

/*****************************************************************************/

static int
private_object_client (LIExtModule* self,
                       LIEngObject* object)
{
	LIAlgPtrdicIter iter;
	LIExtSkills* skills;

	LIALG_PTRDIC_FOREACH (iter, self->dictionary)
	{
		skills = iter.value;
		if (skills->owner == object)
		{
			liext_skills_set_owner (skills, NULL);
			liext_skills_set_owner (skills, object);
		}
	}

	return 1;
}

static int
private_tick (LIExtModule* self,
              float        secs)
{
	LIAlgPtrdicIter iter;
	LIExtSkills* skills;

	LIALG_PTRDIC_FOREACH (iter, self->dictionary)
	{
		skills = iter.value;
		liext_skills_update (skills, secs);
	}

	return 1;
}

static int
private_vision_show (LIExtModule* self,
                     LIEngObject* object,
                     LIEngObject* target)
{
	int perm;
	LIAlgStrdicIter iter;
	LIArcWriter* writer;
	LIExtSkill* skill;
	LIExtSkills* skills;

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
	LIALG_STRDIC_FOREACH (iter, skills->skills)
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
	liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	liarc_writer_free (writer);

	return 1;
}

/** @} */
/** @} */
/** @} */
