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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtEvents Events
 * @{
 */

#include "ext-module.h"

static int
private_login (LIExtModule* self,
               LIEngObject* object,
               const char*  name,
               const char*  pass);

static int
private_logout (LIExtModule* self,
                LIEngObject* object);

static int
private_motion (LIExtModule* self,
                LIEngObject* object);

static int
private_packet (LIExtModule* self,
                LIEngObject* object,
                LIArcReader* packet);

static int
private_tick (LIExtModule* self,
              float        secs);

static int
private_visibility (LIExtModule* self,
                    LIEngObject* object,
                    int          visible);

/*****************************************************************************/

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Events",
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
	self->program = program;

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "client-login", 0, private_login, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "client-logout", 0, private_logout, self, self->calls + 3) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-motion", 0, private_motion, self, self->calls + 4) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "client-packet", 5, private_packet, self, self->calls + 5) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_tick, self, self->calls + 7) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-visibility", 0, private_visibility, self, self->calls + 8))
	{
		liext_module_free (self);
		return NULL;
	}

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int
private_login (LIExtModule* self,
               LIEngObject* object,
               const char*  name,
               const char*  pass)
{
	limai_program_event (self->program, "login",
		"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	return 1;
}

static int
private_logout (LIExtModule* self,
                LIEngObject* object)
{
	limai_program_event (self->program, "logout",
		"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	return 1;
}

static int
private_motion (LIExtModule* self,
                LIEngObject* object)
{
	limai_program_event (self->program, "object-motion",
		"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	return 1;
}

static int
private_packet (LIExtModule* self,
                LIEngObject* object,
                LIArcReader* packet)
{
	int type;
	LIScrData* data0;

	type = ((uint8_t*) packet->buffer)[0];
	data0 = liscr_packet_new_readable (self->program->script, packet);
	limai_program_event (self->program, "packet",
		"object", LISCR_SCRIPT_OBJECT, object->script,
		"message", LISCR_TYPE_INT, type,
		"packet", LISCR_SCRIPT_PACKET, data0, NULL);
	if (data0 != NULL)
		liscr_data_unref (data0, NULL);

	return 1;
}

static int
private_tick (LIExtModule* self,
              float        secs)
{
	limai_program_event (self->program, "tick",
		"secs", LISCR_TYPE_FLOAT, secs, NULL);
	return 1;
}

static int
private_visibility (LIExtModule* self,
                    LIEngObject* object,
                    int          visible)
{
	limai_program_event (self->program, "object-visibility",
		"object", LISCR_SCRIPT_OBJECT, object->script,
		"visible", LISCR_TYPE_BOOLEAN, visible, NULL);
	return 1;
}

/** @} */
/** @} */
