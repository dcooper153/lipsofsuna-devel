/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvscr Script
 * @{
 * \addtogroup lisrvscrObject Object
 * @{
 */

#include <network/lips-network.h>
#include <script/lips-script.h>
#include <server/lips-server.h>

#define MAGICPTR_CONTACT_CALLBACK (NULL + 2)

static void
private_contact_callback (liphyObject*  object,
                          liphyContact* contact)
{
	liscrData* data;
	liengObject* engobj = liphy_object_get_userdata (object);
	lisrvServer* server = lieng_engine_get_userdata (engobj->engine, LIENG_DATA_SERVER);
	liscrScript* script = server->script;

	/* Push callback. */
	liscr_pushdata (script->lua, engobj->script);
	lua_getfield (script->lua, -1, "contact_callback");

	/* Push object. */
	lua_pushvalue (script->lua, -2);
	lua_remove (script->lua, -3);

	/* Push contact. */
	lua_newtable (script->lua);

	/* Convert impulse. */
	lua_pushnumber (script->lua, contact->impulse);
	lua_setfield (script->lua, -2, "impulse");

	/* Convert object. */
	if (contact->object != NULL)
	{
		engobj = liphy_object_get_userdata (contact->object);
		if (engobj != NULL && engobj->script != NULL)
		{
			liscr_pushdata (script->lua, engobj->script);
			lua_setfield (script->lua, -2, "object");
		}
	}

	/* Convert point. */
	data = liscr_vector_new (script, &contact->point);
	if (data != NULL)
	{
		liscr_pushdata (script->lua, data);
		liscr_data_unref (data, NULL);
	}
	else
		lua_pushnil (script->lua);
	lua_setfield (script->lua, -2, "point");

	/* Convert normal. */
	data = liscr_vector_new (script, &contact->normal);
	if (data != NULL)
	{
		liscr_pushdata (script->lua, data);
		liscr_data_unref (data, NULL);
	}
	else
		lua_pushnil (script->lua);
	lua_setfield (script->lua, -2, "normal");

	/* Call function. */
	if (lua_pcall (script->lua, 2, 0, 0) != 0)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "XXXX %s", lua_tostring (script->lua, -1));
		lisys_error_report ();
		lua_pop (script->lua, 1);
	}
}

/*****************************************************************************/

/* @luadoc
 * module "Core.Server.Object"
 * ---
 * -- Create and manipulate server side objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Sets or clears an animation.
 * --
 * -- Arguments:
 * -- channel: Channel number.
 * -- name: Animation name.
 * -- priority: Priority.
 * -- permanent: True if should keep repeating.
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * function Object.animate(self, args)
 */
static void Object_animate (liscrArgs* args)
{
	int repeat = 0;
	int channel = -1;
	float weight = 1.0f;
	const char* animation = NULL;

	liscr_args_gets_string (args, "animation", &animation);
	liscr_args_gets_int (args, "channel", &channel);
	liscr_args_gets_float (args, "weight", &weight);
	liscr_args_gets_bool (args, "permanent", &repeat);
	if (channel < 0 || channel > 254)
		channel = -1;
	lisrv_object_animate (args->self, animation, channel, weight, repeat);
}

/* @luadoc
 * ---
 * -- Disconnects any client controlling the object.
 * --
 * -- @param self Object.
 * function Object.disconnect(self)
 */
static void Object_disconnect (liscrArgs* args)
{
	lisrvObject* object;

	object = LISRV_OBJECT (args->self);
	if (object->client != NULL)
	{
		lisrv_client_free (object->client);
		object->client = NULL;
	}
}

/* @luadoc
 * ---
 * -- Causes the object to emit a sound effect.
 * --
 * -- Arguments:
 * -- effect: Effect name. (required)
 * -- flags: Flags.
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * function Object.effect(self, args)
 */
static void Object_effect (liscrArgs* args)
{
	int flags = LI_EFFECT_DEFAULT;
	const char* name;

	if (liscr_args_gets_string (args, "effect", &name))
	{
		liscr_args_gets_int (args, "flags", &flags);
		lisrv_object_effect (args->self, name, flags);
	}
}

/* @luadoc
 * ---
 * -- Creates a hinge constraint.
 * --
 * -- Arguments:
 * -- position: Position vector.
 * -- axis: Axis of rotation.
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * function Object.insert_hinge_constraint(self, args)
 */
static void Object_insert_hinge_constraint (liscrArgs* args)
{
	liengObject* self;
	limatVector pos;
	limatVector axis = { 0.0f, 1.0f, 0.0f };

	if (liscr_args_gets_vector (args, "position", &pos))
	{
		liscr_args_gets_vector (args, "axis", &axis);
		self = args->self;
		liphy_constraint_new_hinge (self->engine->physics, self->physics, &pos, &axis, 0, 0.0f, 0.0f);
	}
}

/* @luadoc
 * ---
 * -- Creates a new object.
 * --
 * -- @param self Object class.
 * -- @param args Arguments.
 * -- @return New object.
 * function Object.new(self, args)
 */
static void Object_new (liscrArgs* args)
{
	int realize = 0;
	liengObject* self;
	lisrvServer* server;

	/* Allocate self. */
	server = liscr_class_get_userdata (args->clss, LISRV_SCRIPT_OBJECT);
	self = lieng_object_new (server->engine, NULL, LIPHY_CONTROL_MODE_RIGID, 
		lisrv_server_get_unique_object (server));
	if (self == NULL)
		return;

	/* Initialize userdata. */
	liscr_args_call_setters_except (args, self->script, "realized");
	liscr_args_gets_bool (args, "realized", &realize);
	liscr_args_seti_data (args, self->script);
	lieng_object_set_realized (self, realize);
}

/* @luadoc
 * ---
 * -- Purges the object from the database.
 * --
 * -- @param self Object.
 * function Object.purge(self)
 */
static void Object_purge (liscrArgs* args)
{
	lisrv_object_purge (args->self);
}

/* @luadoc
 * ---
 * -- Sends a network packet to the client controlling the object.
 * --
 * -- Arguments:
 * -- packet: Packet. (required)
 * -- reliable: Boolean.
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * function Object.send(self, args)
 */
static void Object_send (liscrArgs* args)
{
	int reliable = 1;
	liscrData* packet;
	liscrPacket* data;
	lisrvClient* client;

	if (liscr_args_gets_data (args, "packet", LICOM_SCRIPT_PACKET, &packet))
	{
		liscr_args_gets_bool (args, "reliable", &reliable);
		client = LISRV_OBJECT (args->self)->client;
		data = packet->data;
		if (client != NULL && data->writer != NULL)
		{
			if (reliable)
				lisrv_client_send (client, data->writer, GRAPPLE_RELIABLE);
			else
				lisrv_client_send (client, data->writer, 0);
		}
	}
}

/* @luadoc
 * ---
 * -- Swaps the clients of the objects.
 * --
 * -- Switches the clients of the passed objects so that the client of the first
 * -- object controls the second object and vice versa. This is typically used when
 * -- you need to destroy the object of a player without disconnecting the client.
 * --
 * -- Argumens:
 * -- object: Object.
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * function Object.swap_clients(self, args)
 */
static void Object_swap_clients (liscrArgs* args)
{
	liscrData* object;

	if (liscr_args_gets_data (args, "object", LICOM_SCRIPT_OBJECT, &object))
		lisrv_object_swap (args->self, object->data);
}

/* @luadoc
 * ---
 * -- Sweeps a sphere relative to the object.
 * --
 * -- Arguments:
 * -- start: Start point vector. (required)
 * -- end: End point vector. (required)
 * -- radius: Sphere radius.
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * -- @return Table with point, normal, and object. Nil if not found.
 * function Object.sweep_sphere(self, args)
 */
static void Object_sweep_sphere (liscrArgs* args)
{
	float radius = 0.5f;
	liengObject* object;
	limatVector start;
	limatVector end;
	liphyCollision result;

	if (!liscr_args_gets_vector (args, "start", &start) ||
	    !liscr_args_gets_vector (args, "end", &end))
		return;
	liscr_args_gets_float (args, "radius", &radius);
	object = args->self;

	if (liphy_object_sweep_sphere (object->physics, &start, &end, radius, &result))
	{
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
		liscr_args_sets_float (args, "fraction", result.fraction);
		liscr_args_sets_vector (args, "point", &result.point);
		liscr_args_sets_vector (args, "normal", &result.normal);
		if (result.object != NULL)
		{
			object = liphy_object_get_userdata (result.object);
			if (object != NULL && object->script != NULL)
				liscr_args_sets_data (args, "object", object->script);
		}
	}
}

/* @luadoc
 * ---
 * -- Function to be called every time the object collides something.
 * -- @name Object.contact_callback
 * -- @class table
 */
static void Object_getter_contact_callback (liscrArgs* args)
{
	lua_pushlightuserdata (args->lua, MAGICPTR_CONTACT_CALLBACK);
	lua_gettable (args->lua, 1);
	liscr_args_seti_stack (args);
}
static void Object_setter_contact_callback (liscrArgs* args)
{
	liengObject* self = args->self;

	if (lua_type (args->lua, 3) == LUA_TFUNCTION)
	{
		liphy_object_set_contact_call (self->physics, private_contact_callback);
		lua_pushlightuserdata (args->lua, MAGICPTR_CONTACT_CALLBACK);
		lua_pushvalue (args->lua, 3);
		lua_settable (args->lua, 1);
	}
	else
	{
		liphy_object_set_contact_call (self->physics, NULL);
		lua_pushlightuserdata (args->lua, MAGICPTR_CONTACT_CALLBACK);
		lua_pushnil (args->lua);
		lua_settable (args->lua, 1);
	}
}

/* @luadoc
 * ---
 * -- Client attached flag.
 * -- @name Object.client
 * -- @class table
 */
static void Object_getter_client (liscrArgs* args)
{
	liscr_args_seti_bool (args, LISRV_OBJECT (args->self)->client != NULL);
}

/* @luadoc
 * ---
 * -- Custom deserialization function.
 * --
 * -- If the function exists, the server calls it when any object is loaded.
 * -- The function is given three arguments: the object, a type string, and a
 * -- data string. The user is free to interpret the strings the way he wants.
 * --
 * -- @name Object.read_cb
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Custom serialization function.
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
lisrvObjectScript (liscrClass* self,
                   void*       data)
{
	liscr_class_inherit (self, licomObjectScript, NULL);
	liscr_class_set_userdata (self, LISRV_SCRIPT_OBJECT, data);
	liscr_class_insert_interface (self, LISRV_SCRIPT_OBJECT);
	liscr_class_insert_mfunc (self, "animate", Object_animate);
	liscr_class_insert_mfunc (self, "disconnect", Object_disconnect);
	liscr_class_insert_mfunc (self, "effect", Object_effect);
	liscr_class_insert_mfunc (self, "insert_hinge_constraint", Object_insert_hinge_constraint);
	liscr_class_insert_cfunc (self, "new", Object_new);
	liscr_class_insert_mfunc (self, "purge", Object_purge);
	liscr_class_insert_mfunc (self, "send", Object_send);
	liscr_class_insert_mfunc (self, "swap_clients", Object_swap_clients);
	liscr_class_insert_mfunc (self, "sweep_sphere", Object_sweep_sphere);
	liscr_class_insert_mvar (self, "client", Object_getter_client, NULL);
	liscr_class_insert_mvar (self, "contact_callback", Object_getter_contact_callback, Object_setter_contact_callback);
}

/** @} */
/** @} */
/** @} */
