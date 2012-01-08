/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

static void private_light_free (
	LIExtLight* self)
{
	liren_render_light_free (self->module->render, self->id);
	lisys_free (self);
}

/*****************************************************************************/

static void Light_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIExtLight* self;
	LIScrData* data;
	const float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float equation[3] = { 1.0f, 1.0f, 1.0f };

	/* Allocate self. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_LIGHT);
	self = lisys_calloc (1, sizeof (LIExtLight));
	if (self == NULL)
		return;
	self->module = module;

	/* Allocate the light data. */
	self->id = liren_render_light_new (module->client->render, black, white, white, equation, M_PI, 0.0f, 0);
	if (!self->id)
	{
		lisys_free (self);
		return;
	}

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_LIGHT, private_light_free);
	if (data == NULL)
	{
		liren_render_light_free (module->render, self->id);
		lisys_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Light_set_ambient (LIScrArgs* args)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	LIExtLight* light = args->self;

	for (i = 0 ; i < 4 ; i++)
		liscr_args_geti_float (args, i, value + i);
	liren_render_light_set_ambient (light->module->render, light->id, value);
}

static void Light_set_diffuse (LIScrArgs* args)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	LIExtLight* light = args->self;

	for (i = 0 ; i < 4 ; i++)
		liscr_args_geti_float (args, i, value + i);
	liren_render_light_set_diffuse (light->module->render, light->id, value);
}

static void Light_set_directional (LIScrArgs* args)
{
	int value;
	LIExtLight* light = args->self;

	if (liscr_args_geti_bool (args, 0, &value))
		liren_render_light_set_directional (light->module->render, light->id, value);
}

static void Light_set_enabled (LIScrArgs* args)
{
	int value;
	LIExtLight* light = args->self;

	if (liscr_args_geti_bool (args, 0, &value))
		liren_render_light_set_enabled (light->module->render, light->id, value);
}

static void Light_set_equation (LIScrArgs* args)
{
	int i;
	float value[3] = { 1.0f, 0.0f, 0.0f };
	LIExtLight* light = args->self;

	for (i = 0 ; i < 3 ; i++)
		liscr_args_geti_float (args, i, value + i);
	liren_render_light_set_equation (light->module->render, light->id, value);
}

static void Light_set_position (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatVector vector;
	LIExtLight* light = args->self;

	if (liscr_args_geti_vector (args, 0, &vector))
	{
		liren_render_light_get_transform (light->module->render, light->id, &transform);
		transform.position = vector;
		liren_render_light_set_transform (light->module->render, light->id, &transform);
	}
}

static void Light_set_priority (LIScrArgs* args)
{
	float value;
	LIExtLight* light = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		liren_render_light_set_priority (light->module->render, light->id, value);
}

static void Light_set_rotation (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatQuaternion value;
	LIExtLight* light = args->self;

	if (liscr_args_geti_quaternion (args, 0, &value))
	{
		liren_render_light_get_transform (light->module->render, light->id, &transform);
		transform.rotation = value;
		limat_quaternion_normalize (transform.rotation);
		liren_render_light_set_transform (light->module->render, light->id, &transform);
	}
}

static void Light_set_shadow_casting (LIScrArgs* args)
{
	int value;
	LIExtLight* light = args->self;

	if (liscr_args_geti_bool (args, 0, &value))
		liren_render_light_set_shadow (light->module->render, light->id, value);
}

static void Light_set_shadow_far (LIScrArgs* args)
{
	float value;
	LIExtLight* light = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		liren_render_light_set_shadow_far (light->module->render, light->id, LIMAT_MAX (value, LIMAT_EPSILON));
}

static void Light_set_shadow_near (LIScrArgs* args)
{
	float value;
	LIExtLight* light = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		liren_render_light_set_shadow_near (light->module->render, light->id, LIMAT_MAX (value, LIMAT_EPSILON));
}

static void Light_set_specular (LIScrArgs* args)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	LIExtLight* light = args->self;

	for (i = 0 ; i < 4 ; i++)
		liscr_args_geti_float (args, i, value + i);
	liren_render_light_set_specular (light->module->render, light->id, value);
}

static void Light_set_spot_cutoff (LIScrArgs* args)
{
	float value;
	LIExtLight* light = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		liren_render_light_set_spot_cutoff (light->module->render, light->id, LIMAT_CLAMP (value, 0.0f, M_PI));
}

static void Light_set_spot_exponent (LIScrArgs* args)
{
	float value;
	LIExtLight* light = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		liren_render_light_set_spot_exponent (light->module->render, light->id, LIMAT_CLAMP (value, 0.0f, 127.0f));
}

/*****************************************************************************/

void liext_script_light (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_LIGHT, "light_new", Light_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_ambient", Light_set_ambient);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_diffuse", Light_set_diffuse);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_directional", Light_set_directional);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_enabled", Light_set_enabled);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_equation", Light_set_equation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_position", Light_set_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_priority", Light_set_priority);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_rotation", Light_set_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_shadow_casting", Light_set_shadow_casting);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_shadow_far", Light_set_shadow_far);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_shadow_near", Light_set_shadow_near);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_specular", Light_set_specular);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_spot_cutoff", Light_set_spot_cutoff);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_LIGHT, "light_set_spot_exponent", Light_set_spot_exponent);
}

/** @} */
/** @} */
