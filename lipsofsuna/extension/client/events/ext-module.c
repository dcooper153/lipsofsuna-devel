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
 * \addtogroup liextsrvEvents Events
 * @{
 */

#include "ext-module.h"

static int
private_object_model (LIExtModule* self,
                      LIEngObject* object,
                      LIEngModel*  model);

static int
private_object_new (LIExtModule* self,
                    LIEngObject* object);

static int
private_packet (LIExtModule* self,
                int          type,
                LIArcReader* reader);

static int
private_select (LIExtModule*    self,
                LIRenSelection* selection);

static int
private_tick (LIExtModule* self,
              float        secs);

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
	self->client = limai_program_find_component (program, "client");

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "object-model", 2, private_object_model, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-new", 2, private_object_new, self, self->calls + 1) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "packet", 2, private_packet, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "select", 2, private_select, self, self->calls + 3) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "tick", 2, private_tick, self, self->calls + 4))
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
private_object_model (LIExtModule* self,
                      LIEngObject* object,
                      LIEngModel*  model)
{
	if (object->script != NULL && model != NULL)
	{
		limai_program_event (self->client->program, "object-model",
			"object", LISCR_SCRIPT_OBJECT, object->script,
			"model", LISCR_TYPE_STRING, model->name, NULL);
	}

	return 1;
}

static int
private_object_new (LIExtModule* self,
                    LIEngObject* object)
{
	if (object->script != NULL)
	{
		/* Emit object creation event. */
		limai_program_event (self->client->program, "object-new",
			"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	}

	return 1;
}

static int
private_packet (LIExtModule* self,
                int          type,
                LIArcReader* reader)
{
	LIScrData* data0;

	reader->pos = 1;
	data0 = liscr_packet_new_readable (self->client->script, reader);
	limai_program_event (self->client->program, "packet",
		"message", LISCR_TYPE_INT, type,
		"packet", LISCR_SCRIPT_PACKET, data0, NULL);
	if (data0 != NULL)
		liscr_data_unref (data0, NULL);

	return 1;
}

static int
private_select (LIExtModule*    self,
                LIRenSelection* selection)
{
	LIEngObject* object;

	if (selection != NULL)
	{
		object = lieng_engine_find_object (self->client->engine, selection->object);
		if (object != NULL && object->script != NULL)
		{
			limai_program_event (self->client->program, "select",
				"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
			return 1;
		}
	}

	return 1;
}

static int
private_tick (LIExtModule* self,
              float        secs)
{
	limai_program_event (self->client->program, "tick",
		"secs", LISCR_TYPE_FLOAT, secs, NULL);

	return 1;
}

/** @} */
/** @} */
/** @} */
