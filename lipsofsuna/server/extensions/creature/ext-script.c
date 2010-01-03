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
 * \addtogroup liextsrvCreature Creature
 * @{
 */

#include <lipsofsuna/script.h>
#include <lipsofsuna/server.h>
#include "ext-creature.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Creature"
 * ---
 * -- Make your objects move like creatures.
 * -- @name Creature
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new creature logic.
 * --
 * -- @param self Creature class.
 * -- @param args Optional arguments.
 * -- @return New creature logic.
 * function Creature.new(self, args)
 */
static void Creature_new (LIScrArgs* args)
{
	LIExtCreature* self;
	LIExtModule* module;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CREATURE);
	self = liext_creature_new (module->server);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	self->data = liscr_data_new (args->script, self, LIEXT_SCRIPT_CREATURE, liext_creature_free);
	if (self->data == NULL)
	{
		liext_creature_free (self);
		return;
	}
	liscr_args_call_setters (args, self->data);
	liscr_args_seti_data (args, self->data);
	liscr_data_unref (self->data, NULL);
}

/* @luadoc
 * ---
 * -- Control state.
 * -- @name Creature.controls
 * -- @class table
 */
static void Creature_getter_controls (LIScrArgs* args)
{
	liscr_args_seti_int (args, ((LIExtCreature*) args->self)->controls);
}
static void Creature_setter_controls (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
		((LIExtCreature*) args->self)->controls = value;
}

/* @luadoc
 * ---
 * -- Controlled object.
 * -- @name Creature.object
 * -- @class table
 */
static void Creature_getter_object (LIScrArgs* args)
{
	if (((LIExtCreature*) args->self)->object != NULL)
		liscr_args_seti_data (args, ((LIExtCreature*) args->self)->object->script);
}
static void Creature_setter_object (LIScrArgs* args)
{
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_OBJECT, &data))
		liext_creature_set_object (args->self, data->data);
	else
		liext_creature_set_object (args->self, NULL);
}

/*****************************************************************************/

void
liext_script_creature (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_CREATURE, data);
	liscr_class_insert_cfunc (self, "new", Creature_new);
	liscr_class_insert_mvar (self, "controls", Creature_getter_controls, Creature_setter_controls);
	liscr_class_insert_mvar (self, "object", Creature_getter_object, Creature_setter_object);
}

/** @} */
/** @} */
/** @} */
