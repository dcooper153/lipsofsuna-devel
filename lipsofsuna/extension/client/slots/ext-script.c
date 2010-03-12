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

/* @luadoc
 * module "Extension.Client.Slots"
 * ---
 * -- Visualize equipped items.
 * -- @name Slots
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Clears slots of an object.
 * --
 * -- Arguments:
 * -- owner: Object whose slots to modify. (required);
 * --
 * -- @param self Slots class.
 * -- @param args Arguments.
 * function Slots.clear(self, args)
 */
static void Slots_clear (LIScrArgs* args)
{
	LIEngObject* object;
	LIExtModule* module;
	LIScrData* data;

	/* Arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SLOTS);
	if (!liscr_args_gets_data (args, "owner", LISCR_SCRIPT_OBJECT, &data))
		return;
	object = data->data;

	/* Modify slots. */
	liext_module_clear_slots (module, object);
}

/* @luadoc
 * ---
 * -- Attachs a model to a slot.
 * --
 * -- Arguments:
 * -- owner: Object whose slots to modify. (required);
 * -- node: Node name.
 * -- model: Model name.
 * --
 * -- @param self Slots class.
 * -- @param args Arguments.
 * function Slots.set_object(self, args)
 */
static void Slots_set_object (LIScrArgs* args)
{
	const char* node = "#root";
	const char* model = NULL;
	LIEngObject* object;
	LIExtModule* module;
	LIScrData* data;

	/* Arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SLOTS);
	if (!liscr_args_gets_data (args, "owner", LISCR_SCRIPT_OBJECT, &data))
		return;
	object = data->data;
	liscr_args_gets_string (args, "node", &node);
	liscr_args_gets_string (args, "model", &model);

	/* Modify slots. */
	liext_module_set_slots (module, object, node, model);
}

/*****************************************************************************/

void
liext_script_slots (LIScrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SLOTS, data);
	liscr_class_insert_cfunc (self, "clear", Slots_clear);
	liscr_class_insert_cfunc (self, "set_object", Slots_set_object);
}

/** @} */
/** @} */
/** @} */
