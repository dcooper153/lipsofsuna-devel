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
 * \addtogroup liextcliSlots Slots      
 * @{
 */

#include "ext-module.h"
#include "ext-slots.h"

static int private_visibility (
	LIExtModule* self,
	LIEngObject* object,
	int          value);

/*****************************************************************************/

LIMaiExtensionInfo liext_slots_info =
{
	LIMAI_EXTENSION_VERSION, "Slots",
	liext_slots_render_new,
	liext_slots_render_free
};

LIExtModule* liext_slots_render_new (
	LIMaiProgram* program)
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
	if (!lical_callbacks_insert (program->callbacks, program->engine, "object-visibility", 0, private_visibility, self, self->calls + 0))
	{
		liext_slots_render_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Slots", liext_script_slots, self);

	return self;
}

void liext_slots_render_free (LIExtModule* self)
{
	LIAlgU32dicIter iter;

	LIALG_U32DIC_FOREACH (iter, self->dictionary)
		liext_slots_free (iter.value);
	lialg_u32dic_free (self->dictionary);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

void liext_slots_render_clear_slots (
	LIExtModule* self,
	LIEngObject* owner)
{
	LIExtSlots* slots;

	slots = lialg_u32dic_find (self->dictionary, owner->id);
	if (slots != NULL)
	{
		lialg_u32dic_remove (self->dictionary, owner->id);
		liext_slots_free (slots);
	}
}

int liext_slots_render_set_slots (
	LIExtModule* self,
	LIEngObject* owner,
	const char*  node,
	const char*  model)
{
	int ret;
	LIEngModel* engmdl;
	LIExtSlots* slots;

	/* Find model. */
	if (model != NULL)
		engmdl = lieng_engine_find_model_by_name (self->client->engine, model);
	else
		engmdl = NULL;

	/* Find or create slots. */
	slots = lialg_u32dic_find (self->dictionary, owner->id);
	if (slots == NULL)
	{
		if (engmdl == NULL)
		{
			lisys_error_set (EINVAL, "no such model");
			return 0;
		}
		slots = liext_slots_new (self, owner);
		if (slots == NULL)
			return 0;
	}

	/* Set slot object. */
	ret = liext_slots_set_slot (slots, node, node, engmdl);
	if (!slots->slots->size)
		liext_slots_render_clear_slots (self, owner);

	return ret;
}

/*****************************************************************************/

static int private_visibility (
	LIExtModule* self,
	LIEngObject* object,
	int          value)
{
	LIAlgU32dicIter iter;
	LIExtSlots* slots;

	if (!value)
	{
		/* Free slots block. */
		liext_slots_render_clear_slots (self, object);

		/* Disown slot objects. */
		LIALG_U32DIC_FOREACH (iter, self->dictionary)
		{
			slots = iter.value;
			liext_slots_clear_object (slots, object);
		}
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
