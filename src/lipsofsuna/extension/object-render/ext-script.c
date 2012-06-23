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
 * \addtogroup LIExtObjectRender ObjectRender
 * @{
 */

#include "lipsofsuna/object.h"
#include "ext-module.h"

static void Object_add_model (LIScrArgs* args)
{
	LIEngModel* model;
	LIExtModule* module;
	LIObjObject* object;
	LIScrData* value;

	/* Get the object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &value))
	{
		model = liscr_data_get_data (value);
		liren_render_object_add_model (module->render, object->id, model->id);
	}
}

static void Object_animate (LIScrArgs* args)
{
	int additive = 0;
	int repeat = 0;
	float repeat_start = 0.0f;
	float repeat_end = -1.0f;
	int channel = -1;
	int keep = 1;
	float fade_in = 0.0f;
	float fade_out = 0.0f;
	float weight = 1.0f;
	float weight_scale = 0.0f;
	float time = 0.0f;
	float time_scale = 1.0f;
	LIObjObject* object;
	LIExtModule* module;
	LIMdlAnimation* animation = NULL;
	LIMdlPoseChannel* chan = NULL;
	LIScrData* data = NULL;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Handle arguments. */
	liscr_args_gets_bool (args, "additive", &additive);
	if (liscr_args_gets_data (args, "animation", LIEXT_SCRIPT_ANIMATION, &data))
		animation = liscr_data_get_data (data);
	liscr_args_gets_int (args, "channel", &channel);
	liscr_args_gets_float (args, "fade_in", &fade_in);
	liscr_args_gets_float (args, "fade_out", &fade_out);
	liscr_args_gets_bool (args, "permanent", &repeat);
	liscr_args_gets_float (args, "repeat_end", &repeat_end);
	liscr_args_gets_float (args, "repeat_start", &repeat_start);
	if (liscr_args_gets_bool (args, "replace", &keep))
		keep = !keep;
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

	/* Set the pose channel arguments. */
	if (animation != NULL)
	{
		chan = limdl_pose_channel_new (animation);
		chan->repeats = repeat? -1 : 1;
		chan->time = time;
		chan->additive = additive;
		chan->repeat_end = repeat_end;
		chan->repeat_start = repeat_start;
		chan->priority_scale = weight_scale;
		chan->priority_transform = weight;
		chan->time_scale = time_scale;
		chan->fade_in = fade_in;
		chan->fade_out = fade_out;
	}

	/* Handle optional per-node weights. */
	if (animation != NULL && liscr_args_gets_table (args, "node_weights"))
	{
		lua_pushnil (args->lua);
		while (lua_next (args->lua, -2) != 0)
		{
			if (lua_type (args->lua, -2) == LUA_TSTRING &&
				lua_type (args->lua, -1) == LUA_TNUMBER)
			{
				limdl_pose_channel_set_node_priority (chan,
					lua_tostring (args->lua, -2), lua_tonumber (args->lua, -1));
			}
			lua_pop (args->lua, 1);
		}
		lua_pop (args->lua, 1);
	}

	/* Set the animation. */
	liren_render_object_channel_animate (module->render, object->id, channel, keep, chan);
	liscr_args_seti_bool (args, 1);

	/* Free the pose channel arguments. */
	if (chan != NULL)
		limdl_pose_channel_free (chan);
}

static void Object_animate_fade (LIScrArgs* args)
{
	int channel = 0;
	float time = LIMDL_POSE_FADE_AUTOMATIC;
	LIObjObject* object;
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

static void Object_find_node (LIScrArgs* args)
{
	const char* name;
	const char* space = "local";
	LIMatTransform transform;
	LIObjObject* object;
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
	LIObjObject* object;
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
	LIObjObject* object;

	/* Get the render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Get arguments. */
	liscr_args_gets_bool (args, "loop", &loop);
	liscr_args_gets_float (args, "time", &start);

	/* Deform the mesh. */
	liren_render_object_particle_animation (module->render, object->id, start, loop);
}

static void Object_remove_model (LIScrArgs* args)
{
	LIEngModel* model;
	LIExtModule* module;
	LIObjObject* object;
	LIScrData* value;

	/* Get the object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &value))
	{
		model = liscr_data_get_data (value);
		liren_render_object_remove_model (module->render, object->id, model->id);
	}
}

static void Object_replace_model (LIScrArgs* args)
{
	LIEngModel* model;
	LIEngModel* model1;
	LIExtModule* module;
	LIObjObject* object;
	LIScrData* value;
	LIScrData* value1;

	/* Get the object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &value) &&
	    liscr_args_geti_data (args, 1, LISCR_SCRIPT_MODEL, &value1))
	{
		model = liscr_data_get_data (value);
		model1 = liscr_data_get_data (value1);
		liren_render_object_replace_model (module->render, object->id, model->id, model1->id);
	}
}

static void Object_set_effect (LIScrArgs* args)
{
	float params[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const char* shader = NULL;
	LIExtModule* module;
	LIObjObject* object;

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
	LIObjObject* object;

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
	LIObjObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Get arguments. */
	if (liscr_args_geti_bool (args, 0, &value))
		liren_render_object_set_particle_emitting (module->render, object->id, value);
}

static void Object_set_render_distance (LIScrArgs* args)
{
	float value;
	LIExtModule* module;
	LIObjObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		liren_render_object_set_render_distance (module->render, object->id, value);
	else
		liren_render_object_set_render_distance (module->render, object->id, -1.0f);
}

static void Object_get_render_loaded (LIScrArgs* args)
{
	LIExtModule* module;
	LIObjObject* object = args->self;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	if (liren_render_object_get_loaded (module->render, object->id))
		liscr_args_seti_bool (args, 1);
}

static void Object_set_shadow_casting (LIScrArgs* args)
{
	int value;
	LIExtModule* module;
	LIObjObject* object;

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
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_add_model", Object_add_model);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_animate", Object_animate);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_animate_fade", Object_animate_fade);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_find_node", Object_find_node);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_animation", Object_get_animation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_particle_animation", Object_particle_animation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_remove_model", Object_remove_model);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_replace_model", Object_replace_model);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_particle", Object_set_particle);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_particle_emitting", Object_set_particle_emitting);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_effect", Object_set_effect);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_render_distance", Object_set_render_distance);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_render_loaded", Object_get_render_loaded);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_shadow_casting", Object_set_shadow_casting);
}

/** @} */
/** @} */
