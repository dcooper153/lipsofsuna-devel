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
 * \addtogroup liser Server
 * @{
 * \addtogroup liserscr Script
 * @{
 * \addtogroup liserscrObject Object
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/server.h>

/* @luadoc
 * module "Core.Server.Object"
 * --- Create and manipulate server side objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new object.
 * --
 * -- @param clss Object class.
 * -- @param args Arguments.<ul>
 * --   <li>id: Load the object with this ID from the database.</li>
 * --   <li>purge: Remove from the database after loading.</li></ul>
 * -- @return New object.
 * function Object.new(clss, args)
 */
static void Object_new (LIScrArgs* args)
{
	int id;
	int purge;
	int realize = 0;
	LIEngObject* self;
	LIMaiProgram* program;
	LISerServer* server;

	/* Handle loading. */
	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);
	server = limai_program_find_component (program, "server");
	if (liscr_args_gets_int (args, "id", &id))
	{
		self = lieng_engine_find_object (program->engine, id);
		if (self == NULL)
		{
			self = lieng_object_new (program->engine, NULL, LIPHY_CONTROL_MODE_RIGID, id);
			if (self == NULL)
				return;
			if (liser_object_serialize (self, server, 0))
				liscr_args_seti_data (args, self->script);
		}
		else
			liscr_args_seti_data (args, self->script);
		if (liscr_args_gets_bool (args, "purge", &purge) && purge)
			liser_object_purge (self, server);
		return;
	}

	/* Allocate self. */
	liscr_script_set_gc (program->script, 0);
	self = lieng_object_new (program->engine, NULL, LIPHY_CONTROL_MODE_RIGID, 0);
	if (self == NULL)
	{
		liscr_script_set_gc (program->script, 1);
		return;
	}

	/* Initialize userdata. */
	liscr_args_call_setters_except (args, self->script, "realized");
	liscr_args_gets_bool (args, "realized", &realize);
	liscr_args_seti_data (args, self->script);
	lieng_object_set_realized (self, realize);
	liscr_script_set_gc (program->script, 1);
}

/* @luadoc
 * --- Purges the object from the database.
 * --
 * -- @param self Object.
 * function Object.purge(self)
 */
static void Object_purge (LIScrArgs* args)
{
	LIMaiProgram* program;
	LISerServer* server;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);
	server = limai_program_find_component (program, "server");
	liser_object_purge (args->self, server);
}

/* @luadoc
 * --- Writes the object to the database.
 * --
 * -- @param self Object.
 * function Object.write(self)
 */
static void Object_write (LIScrArgs* args)
{
	LIMaiProgram* program;
	LISerServer* server;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);
	server = limai_program_find_component (program, "server");
	if (!liser_object_serialize (args->self, server, 1))
		lisys_error_report ();
}

/* @luadoc
 * --- Custom deserialization function.
 * --
 * -- If the function exists, the server calls it when any object is loaded.
 * -- The function is given three arguments: the object, a type string, and a
 * -- data string. The user is free to interpret the strings the way he wants.
 * --
 * -- @name Object.read_cb
 * -- @class table
 */

/* @luadoc
 * --- Custom serialization function.
 * --
 * -- If the function exists, the server calls it when any object is saved.
 * -- The function is given one argument: the object. The function must return
 * -- two strings: a free form type string and a free form data string.
 * --
 * -- @name Object.write_cb
 * -- @class table
 */

/*****************************************************************************/

void
liser_script_object (LIScrClass* self,
                     void*       data)
{
	liscr_class_inherit (self, liscr_script_object, data);
	liscr_class_insert_cfunc (self, "new", Object_new);
	liscr_class_insert_mfunc (self, "purge", Object_purge);
	liscr_class_insert_mfunc (self, "write", Object_write);
}

/** @} */
/** @} */
/** @} */
