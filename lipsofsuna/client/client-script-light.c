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
 * module "Core.Client.Light"
 * --- Create and manipulate light sources.
 * -- @name Light
 * -- @class table
 */

/* @luadoc
 * --- Creates a new light source.
 * --
 * -- @param clss Light class.
 * -- @param args Arguments.
 * -- @return New light source.
 * function Light.new(clss, args)
 */
static void Light_new (LIScrArgs* args)
{
	LICliClient* client;
	LIRenLight* self;
	LIScrData* data;
	const float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float equation[3] = { 1.0f, 1.0f, 1.0f };

	/* Allocate self. */
	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_LIGHT);
	self = liren_light_new (client->scene, color, equation, M_PI, 0.0f, 0);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LICLI_SCRIPT_LIGHT, liren_light_free);
	if (data == NULL)
	{
		liren_light_free (self);
		return;
	}
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- The ambient color of the light source.
 * -- @name Light.ambient
 * -- @class table
 */
static void Light_getter_ambient (LIScrArgs* args)
{
	LIRenLight* light;

	light = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, light->ambient[0]);
	liscr_args_seti_float (args, light->ambient[1]);
	liscr_args_seti_float (args, light->ambient[2]);
	liscr_args_seti_float (args, light->ambient[3]);
}
static void Light_setter_ambient (LIScrArgs* args)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	LIRenLight* light;

	light = args->self;
	for (i = 0 ; i < 4 ; i++)
		liscr_args_geti_float (args, i, value + i);
	memcpy (light->ambient, value, 4 * sizeof (float));
}

/* @luadoc
 * --- The diffuse color of the light source.
 * -- @name Light.color
 * -- @class table
 */
static void Light_getter_color (LIScrArgs* args)
{
	LIRenLight* light;

	light = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, light->diffuse[0]);
	liscr_args_seti_float (args, light->diffuse[1]);
	liscr_args_seti_float (args, light->diffuse[2]);
	liscr_args_seti_float (args, light->diffuse[3]);
}
static void Light_setter_color (LIScrArgs* args)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	LIRenLight* light;

	light = args->self;
	for (i = 0 ; i < 4 ; i++)
		liscr_args_geti_float (args, i, value + i);
	memcpy (light->diffuse, value, 4 * sizeof (float));
}

/* @luadoc
 * --- Enables or disables the light.
 * --
 * -- @name Light.enabled
 * -- @class table
 */
static void Light_getter_enabled (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liren_light_get_enabled (args->self));
}
static void Light_setter_enabled (LIScrArgs* args)
{
	int value;
	LIRenLight* light;

	light = args->self;
	if (liscr_args_geti_bool (args, 0, &value) && value != liren_light_get_enabled (light))
	{
		if (value)
			liren_lighting_insert_light (light->scene->lighting, light);
		else
			liren_lighting_remove_light (light->scene->lighting, light);
	}
}

/* @luadoc
 * --- The attenuation equation of the light source.
 * -- @name Light.equation
 * -- @class table
 */
static void Light_getter_equation (LIScrArgs* args)
{
	LIRenLight* light;

	light = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, light->equation[0]);
	liscr_args_seti_float (args, light->equation[1]);
	liscr_args_seti_float (args, light->equation[2]);
}
static void Light_setter_equation (LIScrArgs* args)
{
	int i;
	float value[3] = { 1.0f, 0.0f, 0.0f };
	LIRenLight* light;

	light = args->self;
	for (i = 0 ; i < 3 ; i++)
		liscr_args_geti_float (args, i, value + i);
	memcpy (light->equation, value, 3 * sizeof (float));
}

/* @luadoc
 * --- Gets or sets the position of the light.
 * --
 * -- @name Light.position
 * -- @class table
 */
static void Light_getter_position (LIScrArgs* args)
{
	LIMatTransform transform;

	liren_light_get_transform (args->self, &transform);
	liscr_args_seti_vector (args, &transform.position);
}
static void Light_setter_position (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
	{
		liren_light_get_transform (args->self, &transform);
		transform.position = vector;
		liren_light_set_transform (args->self, &transform);
	}
}

/*****************************************************************************/

void
licli_script_light (LIScrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_LIGHT, data);
	liscr_class_insert_cfunc (self, "new", Light_new);
	liscr_class_insert_mvar (self, "ambient", Light_getter_ambient, Light_setter_ambient);
	liscr_class_insert_mvar (self, "color", Light_getter_color, Light_setter_color);
	liscr_class_insert_mvar (self, "enabled", Light_getter_enabled, Light_setter_enabled);
	liscr_class_insert_mvar (self, "equation", Light_getter_equation, Light_setter_equation);
	liscr_class_insert_mvar (self, "position", Light_getter_position, Light_setter_position);
}

/** @} */
/** @} */
