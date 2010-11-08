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
 * \addtogroup LIExtObjectPhysics ObjectPhysics
 * @{
 */

#include "ext-module.h"

static void private_contact_callback (
	LIPhyObject*  object,
	LIPhyContact* contact);

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model);

static int private_object_free (
	LIExtModule* self,
	LIEngObject* object);

static int private_object_model (
	LIExtModule* self,
	LIEngObject* object,
	LIEngModel*  model);

static int private_object_new (
	LIExtModule* self,
	LIEngObject* object);

static int private_object_transform (
	LIExtModule* self,
	LIEngObject* object);

static int private_object_visibility (
	LIExtModule* self,
	LIEngObject* object,
	int          value);

static void private_physics_transform (
	LIExtModule* self,
	LIPhyObject* object);

static int private_tick (
	LIExtModule* self,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_object_physics_info =
{
	LIMAI_EXTENSION_VERSION, "ObjectPhysics",
	liext_object_physics_new,
	liext_object_physics_free
};

LIExtModule* liext_object_physics_new (
	LIMaiProgram* program)
{
	LIExtModule* self;
	LIScrClass* clss;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Make sure the physics extension is loaded. */
	if (!limai_program_insert_extension (program, "physics"))
	{
		liext_object_physics_free (self);
		return NULL;
	}

	/* Find the physics manager. */
	self->physics = limai_program_find_component (program, "physics");
	if (self->physics == NULL)
	{
		liext_object_physics_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "model-changed", -65535, private_model_changed, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "model-free", -65535, private_model_free, self, self->calls + 1) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "model-new", -65535, private_model_new, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-free", -65535, private_object_free, self, self->calls + 3) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-model", -65535, private_object_model, self, self->calls + 4) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-new", -65535, private_object_new, self, self->calls + 5) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-transform", -65535, private_object_transform, self, self->calls + 6) ||
	    !lical_callbacks_insert (program->callbacks, self->physics, "object-transform", -65535, private_physics_transform, self, self->calls + 7) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-visibility", -65535, private_object_visibility, self, self->calls + 8) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "tick", -65535, private_tick, self, self->calls + 9))
	{
		liext_object_physics_free (self);
		return NULL;
	}

	/* Extend the object class. */
	clss = liscr_script_find_class (program->script, "Object");
	if (clss != NULL)
		liext_script_object (clss, self);

	return self;
}

void liext_object_physics_free (
	LIExtModule* self)
{
	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	lisys_free (self);
}

/*****************************************************************************/

static void private_contact_callback (
	LIPhyObject*  object,
	LIPhyContact* contact)
{
	LIScrData* data;
	LIEngObject* engobj = liphy_object_get_userdata (object);
	LIMaiProgram* program = lieng_engine_get_userdata (engobj->engine);
	LIScrScript* script = program->script;
	lua_State* lua = liscr_script_get_lua (script);

	/* Push callback. */
	liscr_pushdata (lua, engobj->script);
	lua_getfield (lua, -1, "contact_cb");
	if (lua_type (lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (lua, 2);
		return;
	}

	/* Push object. */
	lua_pushvalue (lua, -2);
	lua_remove (lua, -3);

	/* Push contact. */
	lua_newtable (lua);

	/* Convert impulse. */
	lua_pushnumber (lua, contact->impulse);
	lua_setfield (lua, -2, "impulse");

	/* Convert object. */
	if (contact->object != NULL)
	{
		engobj = liphy_object_get_userdata (contact->object);
		if (engobj != NULL && engobj->script != NULL)
		{
			liscr_pushdata (lua, engobj->script);
			lua_setfield (lua, -2, "object");
		}
	}

	/* Convert point. */
	data = liscr_vector_new (script, &contact->point);
	if (data != NULL)
	{
		liscr_pushdata (lua, data);
		liscr_data_unref (data);
	}
	else
		lua_pushnil (lua);
	lua_setfield (lua, -2, "point");

	/* Convert normal. */
	data = liscr_vector_new (script, &contact->normal);
	if (data != NULL)
	{
		liscr_pushdata (lua, data);
		liscr_data_unref (data);
	}
	else
		lua_pushnil (lua);
	lua_setfield (lua, -2, "normal");

	/* Call function. */
	if (lua_pcall (lua, 2, 0, 0) != 0)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "Object.contact_cb: %s", lua_tostring (lua, -1));
		lisys_error_report ();
		lua_pop (lua, 1);
	}
}

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model)
{
	if (model->physics != NULL)
		liphy_shape_set_model (model->physics, model->model);

	return 1;
}

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model)
{
	if (model->physics != NULL)
	{
		liphy_shape_free (model->physics);
		model->physics = NULL;
	}

	return 1;
}

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIPhyShape* shape;

	/* Create collision shape. */
	shape = liphy_shape_new (self->physics);
	if (shape == NULL)
		return 1;
	liphy_shape_add_model (shape, model->model, NULL, 1.0f);
	model->physics = shape;

	return 1;
}

static int private_object_free (
	LIExtModule* self,
	LIEngObject* object)
{
	LIPhyObject* phyobj;

	/* Find physics object. */
	phyobj = liphy_physics_find_object (self->physics, object->id);
	if (phyobj == NULL)
		return 1;

	/* Free it. */
	liphy_object_free (phyobj);

	return 1;
}

static int private_object_model (
	LIExtModule* self,
	LIEngObject* object,
	LIEngModel*  model)
{
	LIPhyObject* phyobj;

	/* Find physics object. */
	phyobj = liphy_physics_find_object (self->physics, object->id);
	if (phyobj == NULL)
		return 1;

	/* Set its collision shape. */
	if (model != NULL)
		liphy_object_set_shape (phyobj, model->physics);
	else
		liphy_object_set_shape (phyobj, NULL);

	return 1;
}

static int private_object_new (
	LIExtModule* self,
	LIEngObject* object)
{
	LIPhyObject* phyobj;

	/* Initialize physics. */
	phyobj = liphy_object_new (self->physics, object->id, NULL, LIPHY_CONTROL_MODE_STATIC);
	if (phyobj != NULL)
	{
		liphy_object_set_userdata (phyobj, object);
		liphy_object_set_contact_call (phyobj, private_contact_callback);
	}

	return 1;
}

static int private_object_transform (
	LIExtModule* self,
	LIEngObject* object)
{
	LIPhyObject* phyobj;
	LIMatTransform transform;

	/* Find physics object. */
	phyobj = liphy_physics_find_object (self->physics, object->id);
	if (phyobj == NULL)
		return 1;

	/* Set its transformation. */
	if (!self->silence)
	{
		lieng_object_get_transform (object, &transform);
		liphy_object_set_transform (phyobj, &transform);
	}

	return 1;
}

static void private_physics_transform (
	LIExtModule* self,
	LIPhyObject* object)
{
	LIEngObject* obj;
	LIMatTransform transform;

	/* Find physics object. */
	obj = liphy_object_get_userdata (object);
	if (obj == NULL || obj->sector == NULL)
		return;

	/* Copy the transformation to the engine object. We set the silence
	   variable here to prevent the object-transform event by the engine
	   object from causing a feedback loop. */
	self->silence++;
	liphy_object_get_transform (object, &transform);
	lieng_object_set_transform (obj, &transform);
	lieng_object_moved (obj);
	self->silence--;
}

static int private_object_visibility (
	LIExtModule* self,
	LIEngObject* object,
	int          value)
{
	LIMatTransform transform;
	LIPhyObject* phyobj;

	/* Find physics object. */
	phyobj = liphy_physics_find_object (self->physics, object->id);
	if (phyobj == NULL)
		return 1;

	/* Set its visibility. */
	if (value)
	{
		lieng_object_get_transform (object, &transform);
		liphy_object_set_transform (phyobj, &transform);
	}
	liphy_object_set_realized (phyobj, value);

	return 1;
}

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	/* Update physics. */
	liphy_physics_update (self->physics, secs);

	return 1;
}

/** @} */
/** @} */
