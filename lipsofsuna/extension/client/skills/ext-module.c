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

#include <lipsofsuna/client.h>
#include "ext-module.h"
#include "ext-skills.h"

static int
private_visibility (LIExtModule* self,
                    LIEngObject* object,
                    int          value);

static int
private_packet (LIExtModule* module,
                int          type,
                LIArcReader* reader);

static int
private_packet_diff (LIExtModule* module,
                     LIArcReader* reader);

static int
private_packet_reset (LIExtModule* module,
                      LIArcReader* reader);

/*****************************************************************************/

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Skills",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->client = limai_program_find_component (program, "client");
	self->dictionary = lialg_u32dic_new ();
	if (self->dictionary == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "packet", 0, private_packet, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-visibility", 0, private_visibility, self, self->calls + 1))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Skills", liext_script_skills, self);
	liscr_script_create_class (program->script, "SkillWidget", liext_script_skill_widget, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	LIAlgU32dicIter iter;

	assert (self->dictionary->size == 0);

	LIALG_U32DIC_FOREACH (iter, self->dictionary)
		liext_skills_free (iter.value);
	lialg_u32dic_free (self->dictionary);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

LIExtSkills*
liext_module_find_skills (LIExtModule* self,
                          uint32_t     id)
{
	return lialg_u32dic_find (self->dictionary, id);
}

/*****************************************************************************/

static int
private_packet (LIExtModule* self,
                int          type,
                LIArcReader* reader)
{
	reader->pos = 1;
	switch (type)
	{
		case LIEXT_SKILLS_PACKET_RESET:
			private_packet_reset (self, reader);
			break;
		case LIEXT_SKILLS_PACKET_DIFF:
			private_packet_diff (self, reader);
			break;
	}

	return 1;
}

static int
private_packet_diff (LIExtModule* self,
                     LIArcReader* reader)
{
	uint32_t id;
	float value;
	float maximum;
	char* name;
	LIExtSkills* skills;

	/* Find or create skill block. */
	if (!liarc_reader_get_uint32 (reader, &id))
		return 0;
	skills = lialg_u32dic_find (self->dictionary, id);
	if (skills == NULL)
	{
		skills = liext_skills_new (self, id);
		if (skills == NULL)
			return 0;
		if (!lialg_u32dic_insert (self->dictionary, id, skills))
		{
			liext_skills_unref (skills);
			return 0;
		}
	}

	/* Insert skills. */
	while (!liarc_reader_check_end (reader))
	{
		if (!liarc_reader_get_text (reader, "", &name) ||
		    !liarc_reader_get_float (reader, &value) ||
		    !liarc_reader_get_float (reader, &maximum) ||
		    !liext_skills_set_skill (skills, name, value, maximum))
		{
			lisys_free (name);
			return 0;
		}
		lisys_free (name);
	}

	return 1;
}

static int
private_packet_reset (LIExtModule* self,
                      LIArcReader* reader)
{
	uint32_t id;
	float value;
	float maximum;
	char* name;
	LIExtSkills* skills;

	/* Create or clear skill block. */
	if (!liarc_reader_get_uint32 (reader, &id))
		return 0;
	skills = lialg_u32dic_find (self->dictionary, id);
	if (skills == NULL)
	{
		skills = liext_skills_new (self, id);
		if (skills == NULL)
			return 0;
		if (!lialg_u32dic_insert (self->dictionary, id, skills))
		{
			liext_skills_unref (skills);
			return 0;
		}
	}
	else
		liext_skills_clear (skills);

	/* Insert skills. */
	while (!liarc_reader_check_end (reader))
	{
		if (!liarc_reader_get_text (reader, "", &name) ||
		    !liarc_reader_get_float (reader, &value) ||
		    !liarc_reader_get_float (reader, &maximum) ||
		    !liext_skills_set_skill (skills, name, value, maximum))
		{
			lisys_free (name);
			return 0;
		}
		lisys_free (name);
	}

	return 1;
}

static int
private_visibility (LIExtModule* self,
                    LIEngObject* object,
                    int          value)
{
	LIExtSkills* skills;

	/* Free skill block. */
	if (!value)
	{
		skills = lialg_u32dic_find (self->dictionary, object->id);
		if (skills != NULL)
			liext_skills_unref (skills);
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
