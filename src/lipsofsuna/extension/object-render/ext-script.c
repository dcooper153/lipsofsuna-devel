/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

static void Object_animate (LIScrArgs* args)
{
	int additive = 0;
	int repeat = 0;
	int repeat_start = 0;
	int channel = -1;
	int keep = 0;
	float fade_in = 0.0f;
	float fade_out = 0.0f;
	float weight = 1.0f;
	float weight_scale = 0.0f;
	float time = 0.0f;
	float time_scale = 1.0f;
	const char* animation = NULL;
	const char* node_names[512];
	float node_weights[512];
	int node_num = 0;
	LIEngObject* object;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Handle arguments. */
	liscr_args_gets_bool (args, "additive", &additive);
	liscr_args_gets_string (args, "animation", &animation);
	liscr_args_gets_int (args, "channel", &channel);
	liscr_args_gets_float (args, "fade_in", &fade_in);
	liscr_args_gets_float (args, "fade_out", &fade_out);
	liscr_args_gets_bool (args, "permanent", &repeat);
	liscr_args_gets_int (args, "repeat_start", &repeat_start);
	liscr_args_gets_float (args, "time", &time);
	liscr_args_gets_float (args, "time_scale", &time_scale);
	liscr_args_gets_float (args, "weight", &weight);
	liscr_args_gets_float (args, "weight_scale", &weight_scale);
	if (channel < 1 || channel > 255)
		channel = -1;
	else
		channel--;
	repeat_start = LIMAT_MAX (0, repeat_start);
	time_scale = LIMAT_MAX (0.0f, time_scale);

	/* Handle optional per-node weights. */
	if (animation != NULL && liscr_args_gets_table (args, "node_weights"))
	{
		lua_pushnil (args->lua);
		while (lua_next (args->lua, -2) != 0)
		{
			if (lua_type (args->lua, -2) == LUA_TSTRING &&
				lua_type (args->lua, -1) == LUA_TNUMBER)
			{
				if (node_num < 512)
				{
					node_names[node_num] = lua_tostring (args->lua, -2);
					node_weights[node_num] = lua_tonumber (args->lua, -1);
					node_num++;
				}
			}
			lua_pop (args->lua, 1);
		}
		lua_pop (args->lua, 1);
	}

	liren_render_object_channel_animate (module->render, object->id, channel,
		animation, additive, repeat, repeat_start, keep, fade_in, fade_out,
		weight, weight_scale, time, time_scale, node_names, node_weights, node_num);
	liscr_args_seti_bool (args, 1);
}

static void Object_animate_fade (LIScrArgs* args)
{
	int channel = 0;
	float time = LIMDL_POSE_FADE_AUTOMATIC;
	LIEngObject* object;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	if (!liscr_args_gets_int (args, "channel", &channel))
		return;
	if (liscr_args_gets_float (args, "duration", &time))
		time = LIMAT_MAX (0, time);

	if (channel < 1 || channel > 255)
		return;
	channel--;

	liren_render_object_channel_fade (module->render, object->id, channel, time);
}

static void Object_edit_pose (LIScrArgs* args)
{
	int frame = 0;
	int channel = 0;
	float scale = 1.0f;
	const char* node = NULL;
	LIMatTransform transform;
	LIEngObject* object;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	if (!liscr_args_gets_string (args, "node", &node))
		return;
	if (liscr_args_gets_int (args, "channel", &channel))
	{
		channel--;
		if (channel < 0) channel = 0;
		if (channel > 254) channel = 254;
	}
	if (liscr_args_gets_int (args, "frame", &frame))
	{
		frame--;
		if (frame < 0)
			return;
	}
	transform = limat_transform_identity ();
	liscr_args_gets_quaternion (args, "rotation", &transform.rotation);
	liscr_args_gets_vector (args, "position", &transform.position);
	liscr_args_gets_float (args, "scale", &scale);
	transform.rotation = limat_quaternion_normalize (transform.rotation);

	liren_render_object_channel_edit (module->render, object->id, channel, frame, node, &transform, scale);
}

static void Object_deform_mesh (LIScrArgs* args)
{
	/* FIXME: remove */
}

static void Object_find_node (LIScrArgs* args)
{
	const char* name;
	const char* space = "local";
	LIMatTransform transform;
	LIEngObject* object;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	if (!liscr_args_gets_string (args, "name", &name))
		return;
	liscr_args_gets_string (args, "space", &space);

	/* Get the transformation. */
	if (!liren_render_object_find_node (module->render, object->id, name, !strcmp (space, "world"), &transform))
		return;

	/* Return the transformation. */
	liscr_args_seti_vector (args, &transform.position);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

static void Object_get_animation (LIScrArgs* args)
{
	int chan;
	LIAlgStrdicIter iter;
	LIEngObject* object;
	LIExtModule* module;
	LIMdlPoseChannel* channel;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Check arguments. */
	if (!liscr_args_gets_int (args, "channel", &chan))
		return;
	chan--;
	channel = liren_render_object_channel_get_state (module->render, object->id, chan);
	if (channel == NULL)
		return;

	/* Set animation info. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_string (args, "animation", channel->animation->name);
	liscr_args_sets_int (args, "channel", chan + 1);
	liscr_args_sets_float (args, "fade_in", channel->fade_in);
	liscr_args_sets_float (args, "fade_out", channel->fade_out);
	liscr_args_sets_bool (args, "permanent", channel->repeats == -1);
	liscr_args_sets_float (args, "repeat_start", channel->repeat_start);
	liscr_args_sets_float (args, "time", channel->time);
	liscr_args_sets_float (args, "time_scale", channel->time_scale);
	liscr_args_sets_float (args, "weight", channel->priority_transform);
	liscr_args_sets_float (args, "weight_scale", channel->priority_scale);

	/* Set node weight info. */
	if (channel->weights != NULL)
	{
		lua_newtable (args->lua);
		LIALG_STRDIC_FOREACH (iter, channel->weights)
		{
			lua_pushnumber (args->lua, *((float*) iter.value));
			lua_setfield (args->lua, -2, iter.key);
		}
		liscr_args_sets_stack (args, "node_weights");
	}

	/* Free the copied channel. */
	limdl_pose_channel_free (channel);
}

static void Object_particle_animation (LIScrArgs* args)
{
	int loop = 1;
	float start = 0.0f;
	LIExtModule* module;
	LIEngObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Get arguments. */
	liscr_args_gets_bool (args, "loop", &loop);
	liscr_args_gets_float (args, "time", &start);

	/* Deform the mesh. */
	liren_render_object_particle_animation (module->render, object->id, start, loop);
}

static void Object_set_effect (LIScrArgs* args)
{
	float params[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const char* shader = NULL;
	LIExtModule* module;
	LIEngObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Get arguments. */
	if (!liscr_args_geti_string (args, 0, &shader))
		liscr_args_gets_string (args, "shader", &shader);
	liscr_args_gets_floatv (args, "params", 4, params);

	/* Deform the mesh. */
	liren_render_object_set_effect (module->render, object->id, shader, params);
}

static void Object_set_particle (LIScrArgs* args)
{
	const char* name;
	LIExtModule* module;
	LIEngObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Get arguments. */
	if (liscr_args_geti_string (args, 0, &name))
		liren_render_object_set_particle (module->render, object->id, name);
}

static void Object_set_particle_emitting (LIScrArgs* args)
{
	int value;
	LIExtModule* module;
	LIEngObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Get arguments. */
	if (liscr_args_geti_bool (args, 0, &value))
		liren_render_object_set_particle_emitting (module->render, object->id, value);
}

static void Object_set_shadow_casting (LIScrArgs* args)
{
	int value;
	LIExtModule* module;
	LIEngObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	if (liscr_args_geti_bool (args, 0, &value))
		liren_render_object_set_shadow (module->render, object->id, value);
}

/*****************************************************************************/

void liext_script_render_object (
	LIScrScript* self)
{
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_animate", Object_animate);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_animate_fade", Object_animate_fade);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_edit_pose", Object_edit_pose);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_deform_mesh", Object_deform_mesh);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_find_node", Object_find_node);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_animation", Object_get_animation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_particle_animation", Object_particle_animation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_particle", Object_set_particle);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_particle_emitting", Object_set_particle_emitting);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_effect", Object_set_effect);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_shadow_casting", Object_set_shadow_casting);
}

/** @} */
/** @} */
