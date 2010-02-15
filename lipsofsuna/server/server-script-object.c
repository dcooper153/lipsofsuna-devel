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
static void Object_animate (LIScrArgs* args)
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
	liser_object_animate (args->self, animation, channel, weight, repeat);
}

/* @luadoc
 * ---
 * -- Disconnects any client controlling the object.
 * --
 * -- @param self Object.
 * function Object.disconnect(self)
 */
static void Object_disconnect (LIScrArgs* args)
{
	LISerObject* object;

	object = LISER_OBJECT (args->self);
	if (object->client != NULL)
	{
		liser_client_free (object->client);
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
static void Object_effect (LIScrArgs* args)
{
	int flags = LINET_EFFECT_DEFAULT;
	const char* name;

	if (liscr_args_gets_string (args, "effect", &name))
	{
		liscr_args_gets_int (args, "flags", &flags);
		liser_object_effect (args->self, name, flags);
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
static void Object_insert_hinge_constraint (LIScrArgs* args)
{
	LIEngObject* self;
	LIMatVector pos;
	LIMatVector axis = { 0.0f, 1.0f, 0.0f };

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
 * -- Arguments:
 * -- id: Load the object with this ID from the database.
 * -- purge: Remove from the database after loading.
 * --
 * -- @param self Object class.
 * -- @param args Arguments.
 * -- @return New object.
 * function Object.new(self, args)
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
		self = lieng_engine_find_object (server->engine, id);
		if (self == NULL)
		{
			self = lieng_object_new (server->engine, NULL, LIPHY_CONTROL_MODE_RIGID, id);
			if (self == NULL)
				return;
			if (liser_object_serialize (self, 0))
				liscr_args_seti_data (args, self->script);
		}
		else
			liscr_args_seti_data (args, self->script);
		if (liscr_args_gets_bool (args, "purge", &purge) && purge)
			liser_object_purge (self);
		return;
	}

	/* Allocate self. */
	liscr_script_set_gc (server->script, 0);
	id = liser_server_get_unique_object (server);
	self = lieng_object_new (server->engine, NULL, LIPHY_CONTROL_MODE_RIGID, id);
	if (self == NULL)
	{
		liscr_script_set_gc (server->script, 1);
		return;
	}

	/* Initialize userdata. */
	liscr_args_call_setters_except (args, self->script, "realized");
	liscr_args_gets_bool (args, "realized", &realize);
	liscr_args_seti_data (args, self->script);
	lieng_object_set_realized (self, realize);
	liscr_script_set_gc (server->script, 1);
}

/* @luadoc
 * ---
 * -- Purges the object from the database.
 * --
 * -- @param self Object.
 * function Object.purge(self)
 */
static void Object_purge (LIScrArgs* args)
{
	liser_object_purge (args->self);
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
static void Object_send (LIScrArgs* args)
{
	int reliable = 1;
	LIScrData* packet;
	LIScrPacket* data;
	LISerClient* client;

	if (liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &packet))
	{
		liscr_args_gets_bool (args, "reliable", &reliable);
		client = LISER_OBJECT (args->self)->client;
		data = packet->data;
		if (client != NULL && data->writer != NULL)
		{
			if (reliable)
				liser_client_send (client, data->writer, GRAPPLE_RELIABLE);
			else
				liser_client_send (client, data->writer, 0);
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
static void Object_swap_clients (LIScrArgs* args)
{
	LIScrData* object;

	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &object))
		liser_object_swap (args->self, object->data);
}

/* @luadoc
 * ---
 * -- Sweeps a sphere relative to the object.
 * --
 * -- Arguments:
 * -- src: Start point vector. (required)
 * -- dst: End point vector. (required)
 * -- radius: Sphere radius.
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * -- @return Table with point, normal, and object. Nil if not found.
 * function Object.sweep_sphere(self, args)
 */
static void Object_sweep_sphere (LIScrArgs* args)
{
	float radius = 0.5f;
	LIEngObject* object;
	LIMatVector start;
	LIMatVector end;
	LIPhyCollision result;

	if (!liscr_args_gets_vector (args, "src", &start) ||
	    !liscr_args_gets_vector (args, "dst", &end))
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
 * -- Writes the object to the database.
 * --
 * -- @param self Object.
 * function Object.write(self)
 */
static void Object_write (LIScrArgs* args)
{
	if (!liser_object_serialize (args->self, 1))
		lisys_error_report ();
}

/* @luadoc
 * ---
 * -- Client attached flag.
 * -- @name Object.client
 * -- @class table
 */
static void Object_getter_client (LIScrArgs* args)
{
	liscr_args_seti_bool (args, LISER_OBJECT (args->self)->client != NULL);
}

/* @luadoc
 * ---
 * -- Custom collision response callback.
 * --
 * -- Function to be called every time the object collides with something.
 * --
 * -- @name Object.contact_cb
 * -- @class table
 */

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
liser_script_object (LIScrClass* self,
                     void*       data)
{
	liscr_class_inherit (self, liscr_script_object, data);
	liscr_class_insert_mfunc (self, "animate", Object_animate);
	liscr_class_insert_mfunc (self, "disconnect", Object_disconnect);
	liscr_class_insert_mfunc (self, "effect", Object_effect);
	liscr_class_insert_mfunc (self, "insert_hinge_constraint", Object_insert_hinge_constraint);
	liscr_class_insert_cfunc (self, "new", Object_new);
	liscr_class_insert_mfunc (self, "purge", Object_purge);
	liscr_class_insert_mfunc (self, "send", Object_send);
	liscr_class_insert_mfunc (self, "swap_clients", Object_swap_clients);
	liscr_class_insert_mfunc (self, "sweep_sphere", Object_sweep_sphere);
	liscr_class_insert_mfunc (self, "write", Object_write);
	liscr_class_insert_mvar (self, "client", Object_getter_client, NULL);
}

/** @} */
/** @} */
/** @} */
