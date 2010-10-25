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
 * \addtogroup LIExtRender Render
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "core/render"
 * --- Create and manipulate light sources.
 * -- @name Light
 * -- @class table
 */

/* @luadoc
 * --- Creates a new light source.
 * -- @param clss Light class.
 * -- @param args Arguments.
 * -- @return New light source.
 * function Light.new(clss, args)
 */
static void Light_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenLight* self;
	LIScrData* data;
	const float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float equation[3] = { 1.0f, 1.0f, 1.0f };

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_LIGHT);
	self = liren_light_new (module->client->scene, color, equation, M_PI, 0.0f, 0);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, args->clss, liren_light_free);
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

/* @luadoc
 * --- Gets or sets the rotation of the light.
 * -- @name Light.rotation
 * -- @class table
 */
static void Light_getter_rotation (LIScrArgs* args)
{
	LIMatTransform transform;

	liren_light_get_transform (args->self, &transform);
	liscr_args_seti_quaternion (args, &transform.rotation);
}
static void Light_setter_rotation (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatQuaternion value;

	if (liscr_args_geti_quaternion (args, 0, &value))
	{
		liren_light_get_transform (args->self, &transform);
		transform.rotation = value;
		liren_light_set_transform (args->self, &transform);
	}
}

/* @luadoc
 * --- Enables or disables shadow casting.
 * -- @name Light.shadow_casting
 * -- @class table
 */
static void Light_getter_shadow_casting (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liren_light_get_shadow (args->self));
}
static void Light_setter_shadow_casting (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liren_light_set_shadow (args->self, value);
}

/* @luadoc
 * --- Far place distance of the shadow projection.
 * -- @name Light.shadow_far
 * -- @class table
 */
static void Light_getter_shadow_far (LIScrArgs* args)
{
	LIRenLight* self = args->self;

	liscr_args_seti_float (args, self->shadow_far);
}
static void Light_setter_shadow_far (LIScrArgs* args)
{
	float value;
	LIRenLight* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
	{
		self->shadow_far = LIMAT_MAX (value, LIMAT_EPSILON);
		liren_light_update_projection (self);
	}
}

/* @luadoc
 * --- Near place distance of the shadow projection.
 * -- @name Light.shadow_near
 * -- @class table
 */
static void Light_getter_shadow_near (LIScrArgs* args)
{
	LIRenLight* self = args->self;

	liscr_args_seti_float (args, self->shadow_near);
}
static void Light_setter_shadow_near (LIScrArgs* args)
{
	float value;
	LIRenLight* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
	{
		self->shadow_near = LIMAT_MAX (value, LIMAT_EPSILON);
		liren_light_update_projection (self);
	}
}

/* @luadoc
 * --- Spot cutoff angle of the light, in radians.
 * -- @name Light.spot_cutoff
 * -- @class table
 */
static void Light_getter_spot_cutoff (LIScrArgs* args)
{
	LIRenLight* self = args->self;

	liscr_args_seti_float (args, self->cutoff);
}
static void Light_setter_spot_cutoff (LIScrArgs* args)
{
	float value;
	LIRenLight* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
	{
		self->cutoff = LIMAT_CLAMP (value, 0.0f, M_PI);
		liren_light_update_projection (self);
	}
}

/* @luadoc
 * --- Spot exponent of the light.
 * -- @name Light.spot_cutoff
 * -- @class table
 */
static void Light_getter_spot_exponent (LIScrArgs* args)
{
	LIRenLight* self = args->self;

	liscr_args_seti_float (args, self->exponent);
}
static void Light_setter_spot_exponent (LIScrArgs* args)
{
	float value;
	LIRenLight* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		self->exponent = LIMAT_CLAMP (value, 0.0f, 127.0f);
}

/*****************************************************************************/

void liext_script_light (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_LIGHT, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Light_new);
	liscr_class_insert_mvar (self, "ambient", Light_getter_ambient, Light_setter_ambient);
	liscr_class_insert_mvar (self, "color", Light_getter_color, Light_setter_color);
	liscr_class_insert_mvar (self, "enabled", Light_getter_enabled, Light_setter_enabled);
	liscr_class_insert_mvar (self, "equation", Light_getter_equation, Light_setter_equation);
	liscr_class_insert_mvar (self, "position", Light_getter_position, Light_setter_position);
	liscr_class_insert_mvar (self, "rotation", Light_getter_rotation, Light_setter_rotation);
	liscr_class_insert_mvar (self, "shadow_casting", Light_getter_shadow_casting, Light_setter_shadow_casting);
	liscr_class_insert_mvar (self, "shadow_far", Light_getter_shadow_far, Light_setter_shadow_far);
	liscr_class_insert_mvar (self, "shadow_near", Light_getter_shadow_near, Light_setter_shadow_near);
	liscr_class_insert_mvar (self, "spot_cutoff", Light_getter_spot_cutoff, Light_setter_spot_cutoff);
	liscr_class_insert_mvar (self, "spot_exponent", Light_getter_spot_exponent, Light_setter_spot_exponent);
}

/** @} */
/** @} */
