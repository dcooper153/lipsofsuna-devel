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
 * \addtogroup licliscrObject Object
 * @{
 */

#include <client/lips-client.h>
#include <script/common/lips-common-script.h>
#include "lips-client-script.h"

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Light"
 * ---
 * -- Create and manipulate light sources.
 * -- @name Light
 * -- @class table
 */

static int
Light___gc (lua_State* lua)
{
	lirndLight* light;
	liscrData* self;

	self = liscr_isdata (lua, 1, LICLI_SCRIPT_LIGHT);
	light = self->data;

	lirnd_lighting_remove_light (light->scene->lighting, light);
	lirnd_light_free (light);
	liscr_data_free (self);

	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new light source.
 * --
 * -- @param self Light class.
 * -- @param table Optional table of arguments.
 * -- @return New light source.
 * function Light.new(self, table)
 */
static int
Light_new (lua_State* lua)
{
	licliModule* module;
	lirndLight* light;
	liscrData* self;
	liscrScript* script;
	const float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float equation[3] = { 1.0f, 1.0f, 1.0f };

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_LIGHT);

	/* Allocate light. */
	light = lirnd_light_new (module->engine->scene, color, equation, M_PI, 0.0f, 0);
	if (light == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, light, LICLI_SCRIPT_LIGHT);
	if (self == NULL)
	{
		lirnd_light_free (light);
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	return 1;
}

/* @luadoc
 * ---
 * -- The ambient color of the light source.
 * -- @name Light.ambient
 * -- @class table
 */
static int
Light_getter_ambient (lua_State* lua)
{
	int i;
	lirndLight* light;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	light = data->data;

	lua_newtable (lua);
	for (i = 0 ; i < 4 ; i++)
	{
		lua_pushnumber (lua, i + 1);
		lua_pushnumber (lua, light->ambient[i]);
		lua_settable (lua, -3);
	}

	return 1;
}
static int
Light_setter_ambient (lua_State* lua)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	lirndLight* light;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	luaL_checktype (lua, 3, LUA_TTABLE);
	light = data->data;

	for (i = 0 ; i < 4 ; i++)
	{
		lua_pushnumber (lua, i + 1);
		lua_gettable (lua, 3);
		if (lua_isnumber (lua, -1))
			value[i] = lua_tonumber (lua, -1);
		lua_pop (lua, 1);
	}

	memcpy (light->ambient, value, 4 * sizeof (float));
	return 0;
}

/* @luadoc
 * ---
 * -- The diffuse color of the light source.
 * -- @name Light.color
 * -- @class table
 */
static int
Light_getter_color (lua_State* lua)
{
	int i;
	lirndLight* light;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	light = data->data;

	lua_newtable (lua);
	for (i = 0 ; i < 4 ; i++)
	{
		lua_pushnumber (lua, i + 1);
		lua_pushnumber (lua, light->diffuse[i]);
		lua_settable (lua, -3);
	}

	return 1;
}
static int
Light_setter_color (lua_State* lua)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	lirndLight* light;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	luaL_checktype (lua, 3, LUA_TTABLE);
	light = data->data;

	for (i = 0 ; i < 4 ; i++)
	{
		lua_pushnumber (lua, i + 1);
		lua_gettable (lua, 3);
		if (lua_isnumber (lua, -1))
			value[i] = lua_tonumber (lua, -1);
		lua_pop (lua, 1);
	}

	memcpy (light->diffuse, value, 4 * sizeof (float));
	return 0;
}

/* @luadoc
 * ---
 * -- Enables or disables the light.
 * --
 * -- @name Light.enabled
 * -- @class table
 */
static int
Light_getter_enabled (lua_State* lua)
{
	lirndLight* light;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	light = data->data;

	lua_pushboolean (lua, lirnd_light_get_enabled (light));
	return 1;
}
static int
Light_setter_enabled (lua_State* lua)
{
	int value;
	lirndLight* light;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	value = lua_toboolean (lua, 3);
	light = data->data;

	if (value != lirnd_light_get_enabled (light))
	{
		if (value)
			lirnd_lighting_insert_light (light->scene->lighting, light);
		else
			lirnd_lighting_remove_light (light->scene->lighting, light);
	}
	return 0;
}

/* @luadoc
 * ---
 * -- The attenuation equation of the light source.
 * -- @name Light.equation
 * -- @class table
 */
static int
Light_getter_equation (lua_State* lua)
{
	int i;
	lirndLight* light;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	light = data->data;

	lua_newtable (lua);
	for (i = 0 ; i < 3 ; i++)
	{
		lua_pushnumber (lua, i + 1);
		lua_pushnumber (lua, light->equation[i]);
		lua_settable (lua, -3);
	}

	return 1;
}
static int
Light_setter_equation (lua_State* lua)
{
	int i;
	float value[4] = { 1.0f, 0.0f, 0.0f };
	lirndLight* light;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	luaL_checktype (lua, 3, LUA_TTABLE);
	light = data->data;

	for (i = 0 ; i < 3 ; i++)
	{
		lua_pushnumber (lua, i + 1);
		lua_gettable (lua, 3);
		if (lua_isnumber (lua, -1))
			value[i] = lua_tonumber (lua, -1);
		lua_pop (lua, 1);
	}

	memcpy (light->equation, value, 3 * sizeof (float));
	return 0;
}

/* @luadoc
 * ---
 * -- Gets or sets the position of the light.
 * --
 * -- @name Light.position
 * -- @class table
 */
static int
Light_getter_position (lua_State* lua)
{
	limatTransform transform;
	lirndLight* light;
	liscrData* data;
	liscrData* vector;
	liscrScript* script = liscr_script (lua);

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	light = data->data;

	lirnd_light_get_transform (light, &transform);
	vector = liscr_vector_new (script, &transform.position);
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
Light_setter_position (lua_State* lua)
{
	limatTransform transform;
	lirndLight* light;
	liscrData* data;
	liscrData* vector;

	data = liscr_checkdata (lua, 1, LICLI_SCRIPT_LIGHT);
	vector = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);
	light = data->data;

	lirnd_light_get_transform (light, &transform);
	transform.position = *((limatVector*) vector->data);
	lirnd_light_set_transform (light, &transform);
	return 0;
}

/*****************************************************************************/

void
licliLightScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_LIGHT, data);
	liscr_class_insert_func (self, "__gc", Light___gc);
	liscr_class_insert_func (self, "new", Light_new);
	liscr_class_insert_getter (self, "ambient", Light_getter_ambient);
	liscr_class_insert_getter (self, "color", Light_getter_color);
	liscr_class_insert_getter (self, "enabled", Light_getter_enabled);
	liscr_class_insert_getter (self, "equation", Light_getter_equation);
	liscr_class_insert_getter (self, "position", Light_getter_position);
	liscr_class_insert_setter (self, "ambient", Light_setter_ambient);
	liscr_class_insert_setter (self, "color", Light_setter_color);
	liscr_class_insert_setter (self, "enabled", Light_setter_enabled);
	liscr_class_insert_setter (self, "equation", Light_setter_equation);
	liscr_class_insert_setter (self, "position", Light_setter_position);
}

/** @} */
/** @} */
/** @} */
