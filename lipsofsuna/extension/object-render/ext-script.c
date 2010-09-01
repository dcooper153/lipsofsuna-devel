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
 * \addtogroup LIExtObjectRender ObjectRender
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.ObjectRender"
 * ---
 * -- Control rendering of objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * --- Deforms the mesh of the object according to its animation pose.
 * --
 * -- @param self Object.
 * function Object.deform_mesh(self)
 */
static void Object_deform_mesh (LIScrArgs* args)
{
	LIExtModule* module;
	LIEngObject* engobj;
	LIRenObject* object;

	/* Get render object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_RENDER_OBJECT);
	engobj = args->self;
	object = liren_scene_find_object (module->scene, engobj->id);
	if (object == NULL)
		return;

	/* Deform the mesh. */
	liren_object_deform (object);
}

/* @luadoc
 * --- Starts the particle animation of the object.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>loop: True to loop infinitely.</li>
 * --   <li>time: Animation offset in seconds.</li></ul>
 * function Object.particle_animation(self)
 */
static void Object_particle_animation (LIScrArgs* args)
{
	int loop = 1;
	float start = 0.0f;
	LIExtModule* module;
	LIEngObject* engobj;
	LIRenObject* object;

	/* Get render object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_RENDER_OBJECT);
	engobj = args->self;
	object = liren_scene_find_object (module->scene, engobj->id);
	if (object == NULL)
		return;

	/* Get arguments. */
	liscr_args_gets_bool (args, "loop", &loop);
	liscr_args_gets_float (args, "time", &start);

	/* Deform the mesh. */
	liren_object_particle_animation (object, start, loop);
}

/*****************************************************************************/

void liext_script_render_object (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_RENDER_OBJECT, data);
	liscr_class_insert_mfunc (self, "deform_mesh", Object_deform_mesh);
	liscr_class_insert_mfunc (self, "particle_animation", Object_particle_animation);
}
