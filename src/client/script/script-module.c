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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliscr Script
 * @{
 * \addtogroup licliscrModule Module
 * @{
 */

#include <stdlib.h>
#include <client/lips-client.h>
#include <network/lips-network.h>
#include <script/common/lips-common-script.h>
#include <system/lips-system.h>
#include "lips-client-script.h"

/* @luadoc
 * module "Core.Client.Module"
 * ---
 * -- Access and manipulate the state of the active module.
 * -- @name Module
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Finds an object by ID.
 * --
 * -- @param self Module class.
 * -- @param id Object ID.
 * -- @return Object or nil.
 * function Module.find_object(self, id)
 */
static int
Module_find_object (lua_State* lua)
{
	uint32_t id;
	liengObject* object;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	id = (uint32_t) luaL_checknumber (lua, 2);

	object = lieng_engine_find_object (module->engine, id);
	if (object != NULL)
		liscr_pushdata (lua, object->script);
	else
		lua_pushnil (lua);
	return 1;
}

/* @luadoc
 * ---
 * -- Launches a server and joins it.
 * --
 * -- If a server has already been launched, it is terminated.
 * --
 * -- @param self Module class.
 * -- @param login Optional login name.
 * -- @param password Optional password.
 * -- @return True on success.
 * function Module.host(self, login, password)
 */
static int
Module_host (lua_State* lua)
{
	const char* name = NULL;
	const char* pass = NULL;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	if (lua_gettop (lua) >= 2)
		name = luaL_checkstring (lua, 2);
	if (lua_gettop (lua) >= 3)
		pass = luaL_checkstring (lua, 3);

	if (!licli_module_host (module))
	{
		lisys_error_report ();
		lua_pushboolean (lua, 0);
		return 1;
	}
	if (!licli_module_connect (module, name, pass))
	{
		lisys_error_report ();
		lua_pushboolean (lua, 0);
		return 1;
	}
	lua_pushboolean (lua, 1);
	return 1;
}

/* @luadoc
 * ---
 * -- Joins a server.
 * --
 * -- @param self Module class.
 * -- @param login Optional login name.
 * -- @param password Optional password.
 * -- @return True on success.
 * function Module.join(self, login, password)
 */
static int
Module_join (lua_State* lua)
{
	const char* name = NULL;
	const char* pass = NULL;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	if (lua_gettop (lua) >= 2)
		name = luaL_checkstring (lua, 2);
	if (lua_gettop (lua) >= 3)
		pass = luaL_checkstring (lua, 3);

	if (!licli_module_connect (module, name, pass))
	{
		lisys_error_report ();
		lua_pushboolean (lua, 0);
		return 1;
	}
	lua_pushboolean (lua, 1);
	return 1;
}

/* @luadoc
 * ---
 * -- Pick an object from the screen.
 * --
 * -- @param self Module class.
 * -- @param x Optional X coordinate, default is cursor position.
 * -- @param y Optional Y coordinate, default is cursor position.
 * -- @return Object or nil.
 * function Module.pick(self, x, y)
 */
static int
Module_pick (lua_State* lua)
{
	int x;
	int y;
	licliModule* module;
	liengObject* object;
	lirndSelection result;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	SDL_GetMouseState (&x, &y);
	if (lua_gettop (lua) >= 2)
		x = luaL_checknumber (lua, 2);
	if (lua_gettop (lua) >= 3)
		y = luaL_checknumber (lua, 3);

	if (!licli_module_pick (module, x, y, &result))
	{
		lua_pushnil (lua);
		return 1;
	}
	object = lieng_engine_find_object (module->engine, result.object);
	if (object == NULL || object->script == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	liscr_pushdata (lua, object->script);
	return 1;
}

/* @luadoc
 * ---
 * -- Sends a network packet to the server.
 * --
 * -- @param self Module class.
 * -- @param packet Network packet.
 * function Module.send(self, packet)
 */
static int
Module_send (lua_State* lua)
{
	licliModule* module;
	liscrData* packet;
	liscrPacket* data;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	packet = liscr_checkdata (lua, 2, LICOM_SCRIPT_PACKET);
	data = packet->data;
	luaL_argcheck (lua, data->writer != NULL, 2, "packet is not writable");
	/* TODO: Send flags. */

	licli_module_send (module, data->writer, GRAPPLE_RELIABLE);
	return 0;
}

/* @luadoc
 * ---
 * -- Ambient light color.
 * -- @name Module.ambient
 * -- @class table
 */
static int
Module_setter_ambient (lua_State* lua)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	luaL_checktype (lua, 3, LUA_TTABLE);

	for (i = 0 ; i < 4 ; i++)
	{
		lua_pushnumber (lua, i + 1);
		lua_gettable (lua, 3);
		if (lua_isnumber (lua, -1))
			value[i] = lua_tonumber (lua, -1);
		lua_pop (lua, 1);
	}

	lirnd_lighting_set_ambient (module->engine->render->lighting, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Movement mode flag.
 * -- @name Module.moving
 * -- @class table
 */
static int
Module_getter_moving (lua_State* lua)
{
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	liscr_checkclass (lua, 1, LICLI_SCRIPT_MODULE);

	lua_pushboolean (lua, licli_module_get_moving (module));
	return 1;
}
static int
Module_setter_moving (lua_State* lua)
{
	int value;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	value = lua_toboolean (lua, 3);

	licli_module_set_moving (module, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Music.
 * -- @name Module.music
 * -- @class table
 */
static int
Module_setter_music (lua_State* lua)
{
#ifndef LI_DISABLE_SOUND
	const char* value;
	licliModule* module;
	lisndSample* sample;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	value = luaL_checkstring (lua, 3);

	/* Set sound effect. */
	if (module->client->sound != NULL)
	{
		sample = licli_module_find_sample_by_name (module, value);
		if (sample != NULL)
		{
			/* FIXME: Doesn't work if already playing. */
			lisnd_source_queue_sample (module->music, sample);
			lisnd_source_set_looping (module->music, 1);
			lisnd_source_set_playing (module->music, 1);
		}
	}
#else
	liscr_checkclass (lua, 1, LICLI_SCRIPT_MODULE);
#endif

	return 0;
}

/* @luadoc
 * ---
 * -- Root widget.
 * -- @name Module.root
 * -- @class table
 */
static int
Module_setter_root (lua_State* lua)
{
	licliModule* module;
	liscrData* window;
	liwdgWidget* data;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	if (!lua_isnil (lua, 3))
	{
		window = liscr_checkiface (lua, 3, LICLI_SCRIPT_WIDGET);
		data = window->data;
		luaL_argcheck (lua, data->parent == NULL, 3, "widget already in use");
		luaL_argcheck (lua, data->state == LIWDG_WIDGET_STATE_DETACHED, 3, "widget already in use");
	}
	else
		window = NULL;

	/* Detach old root widget. */
	data = liwdg_manager_get_root (module->widgets);
	if (data != NULL)
	{
		if (liwdg_widget_get_userdata (data))
			liscr_data_unref (liwdg_widget_get_userdata (data), NULL);
		if (!liwdg_manager_remove_window (module->widgets, data))
			return 0;
		liwdg_widget_set_visible (data, 0);
	}
	liwdg_manager_set_root (module->widgets, NULL);

	/* Set new root window. */
	if (window != NULL)
	{
		liscr_data_ref (window, NULL);
		liwdg_manager_set_root (module->widgets, window->data);
	}

	return 0;
}

/* @luadoc
 * ---
 * -- Skybox model.
 * -- @name Module.sky
 * -- @class table
 */
static int
Module_setter_sky (lua_State* lua)
{
#warning Skybox is disabled.
#if 0
	const char* name;
	licliModule* module;
	lirndModel* model;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	name = lua_tostring (lua, 3);

	model = lirnd_render_find_model_by_name (module->render, name);
	if (model != NULL)
		lirnd_render_set_sky (module->render, model);
#endif
	return 0;
}

/* @luadoc
 * ---
 * -- Sun light diffuse color.
 * -- @name Module.sun_color
 * -- @class table
 */
static int
Module_setter_sun_color (lua_State* lua)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	luaL_checktype (lua, 3, LUA_TTABLE);

	for (i = 0 ; i < 4 ; i++)
	{
		lua_pushnumber (lua, i + 1);
		lua_gettable (lua, 3);
		if (lua_isnumber (lua, -1))
			value[i] = lua_tonumber (lua, -1);
		lua_pop (lua, 1);
	}

	lirnd_lighting_set_sun_color (module->engine->render->lighting, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Sun light direction.
 * -- @name Module.sun_direction
 * -- @class table
 */
static int
Module_getter_sun_direction (lua_State* lua)
{
	licliModule* module;
	liscrData* vector;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	liscr_checkclass (lua, 1, LICLI_SCRIPT_MODULE);

	assert (module != NULL);
	assert (module->engine != NULL);

	vector = liscr_vector_new (script, &module->engine->render->lighting->sun.direction);
	if (vector != NULL)
	{
		liscr_pushdata (lua, vector);
		liscr_data_unref (vector, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}
static int
Module_setter_sun_direction (lua_State* lua)
{
	licliModule* module;
	liscrData* vector;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_MODULE);
	vector = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);

	lirnd_render_set_sun (module->engine->render, vector->data);
	return 0;
}

/* @luadoc
 * ---
 * -- Main window title.
 * -- @name Module.title
 * -- @class table
 */
static int
Module_setter_title (lua_State* lua)
{
	const char* value;

	liscr_checkclass (lua, 1, LICLI_SCRIPT_MODULE);
	value = luaL_checkstring (lua, 3);

	SDL_WM_SetCaption (value, value);
	return 0;
}

/*****************************************************************************/

void
licliModuleScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LICLI_SCRIPT_MODULE, data);
	liscr_class_insert_func (self, "find_object", Module_find_object);
	liscr_class_insert_func (self, "host", Module_host);
	liscr_class_insert_func (self, "join", Module_join);
	liscr_class_insert_func (self, "pick", Module_pick);
	liscr_class_insert_func (self, "send", Module_send);
	liscr_class_insert_getter (self, "moving", Module_getter_moving);
	liscr_class_insert_getter (self, "sun_direction", Module_getter_sun_direction);
	liscr_class_insert_setter (self, "ambient", Module_setter_ambient);
	liscr_class_insert_setter (self, "moving", Module_setter_moving);
	liscr_class_insert_setter (self, "music", Module_setter_music);
	liscr_class_insert_setter (self, "root", Module_setter_root);
	liscr_class_insert_setter (self, "sky", Module_setter_sky);
	liscr_class_insert_setter (self, "sun_color", Module_setter_sun_color);
	liscr_class_insert_setter (self, "sun_direction", Module_setter_sun_direction);
	liscr_class_insert_setter (self, "title", Module_setter_title);
}

/** @} */
/** @} */
/** @} */
