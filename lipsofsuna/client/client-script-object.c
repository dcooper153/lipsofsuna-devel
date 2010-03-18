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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliscr Script
 * @{
 */

#include <lipsofsuna/client.h>

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Object"
 * ---
 * -- Create and manipulate client side objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * --- FIXME
 * --
 * -- @param self Object.
 * function Object.emit_particles(self)
 */
static void Object_emit_particles (LIScrArgs* args)
{
	LICliClient* client;
	LIEngObject* object;
	LIRenObject* render;

	object = LIENG_OBJECT (args->self);
	client = lieng_engine_get_userdata (object->engine);
	render = liren_scene_find_object (client->scene, object->id);
	if (render != NULL)
		liren_object_emit_particles (render);
}

/* @luadoc
 * --- Creates a new object.
 * --
 * -- @param self Object class.
 * -- @param args Arguments.<ul>
 * --   <li>id: Create with an explicit ID.</li></ul>
 * -- @return New object.
 * function Object.new(self, args)
 */
static void Object_new (LIScrArgs* args)
{
	int id;
	int realize = 0;
	LIEngObject* self;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);
	liscr_script_set_gc (program->script, 0);

	/* Allocate self. */
	if (liscr_args_gets_int (args, "id", &id))
	{
		self = lieng_engine_find_object (program->engine, id);
		if (self == NULL)
			self = lieng_object_new (program->engine, NULL, LIPHY_CONTROL_MODE_STATIC, id);
	}
	else
		self = lieng_object_new (program->engine, NULL, LIPHY_CONTROL_MODE_STATIC, 0);
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

/*****************************************************************************/

void
licli_script_object (LIScrClass* self,
                     void*       data)
{
	liscr_class_inherit (self, liscr_script_object, data);
	liscr_class_insert_cfunc (self, "new", Object_new);
	liscr_class_insert_mfunc (self, "emit_particles", Object_emit_particles);
}

/** @} */
/** @} */
