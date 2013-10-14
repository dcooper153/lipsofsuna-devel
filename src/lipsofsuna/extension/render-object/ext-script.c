/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIExtRenderObjectRender ObjectRender
 * @{
 */

#include "ext-module.h"
#include "lipsofsuna/extension/image/module.h"

static void RenderObject_new (LIScrArgs* args)
{
	LIExtRenderObject* self;
	LIExtModule* module;
	LIScrData* data;

	/* Find the class data. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);

	/* Allocate the object. */
	self = liext_render_object_new (module);
	if (self == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_RENDER_OBJECT, liext_render_object_free);
	if (data == NULL)
	{
		liext_render_object_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void RenderObject_add_model (LIScrArgs* args)
{
	LIExtRenderModel* model;
	LIExtRenderObject* object;
	LIScrData* value;

	object = args->self;
	if (liscr_args_geti_data (args, 0, LIEXT_SCRIPT_RENDER_MODEL, &value))
	{
		model = liscr_data_get_data (value);
		liren_render_object_add_model (object->render, object->id, model->id);
	}
}

static void RenderObject_add_texture_alias (LIScrArgs* args)
{
	const char* name;
	LIExtRenderObject* self;
	LIImgImage* image;
	LIScrData* value;

	self = args->self;
	if (liscr_args_geti_string (args, 0, &name) &&
	    liscr_args_geti_data (args, 1, LIEXT_SCRIPT_IMAGE, &value))
	{
		image = liscr_data_get_data (value);
		liren_render_object_add_texture_alias (self->render, self->id, name,
			image->width, image->height, image->pixels);
	}
}

static void RenderObject_animate (LIScrArgs* args)
{
	int blend_mode = LIMDL_POSE_BLEND_MIX;
	int repeat = 0;
	float repeat_start = 0.0f;
	float repeat_end = -1.0f;
	int channel = -1;
	int keep = 1;
	float fade_in = 0.0f;
	float fade_out = 0.0f;
	int priority = 0;
	int priority_scale = 0;
	float weight = 1.0f;
	float weight_scale = 0.0f;
	float time = 0.0f;
	float time_scale = 1.0f;
	const char* str;
	LIExtRenderObject* object;
	LIMdlAnimation* animation = NULL;
	LIMdlPoseChannel* chan = NULL;
	LIScrData* data = NULL;

	/* Handle arguments. */
	object = args->self;
	if (liscr_args_gets_string (args, "blend_mode", &str))
	{
		if (!strcmp (str, "add"))
			blend_mode = LIMDL_POSE_BLEND_ADD;
		else if (!strcmp (str, "mix"))
			blend_mode = LIMDL_POSE_BLEND_MIX;
		else if (!strcmp (str, "replace"))
			blend_mode = LIMDL_POSE_BLEND_REPLACE;
	}
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
	liscr_args_gets_int (args, "priority", &priority);
	liscr_args_gets_int (args, "priority_scale", &priority_scale);
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
		chan->repeats = repeat? -1 : 0;
		chan->time = time;
		chan->blend_mode = blend_mode;
		chan->repeat_end = repeat_end;
		chan->repeat_start = repeat_start;
		chan->priority_scale = priority_scale;
		chan->priority_transform = priority;
		chan->weight_scale = weight_scale;
		chan->weight_transform = weight;
		chan->time_scale = time_scale;
		chan->fade_in.duration = fade_in;
		chan->fade_out.duration = fade_out;
		if (liscr_args_gets_string (args, "fade_in_mode", &str))
		{
			if (!strcmp (str, "after start"))
				chan->fade_in.mode = LIMDL_POSE_FADE_IN_AFTER_START;
			else if (!strcmp (str, "before start"))
				chan->fade_in.mode = LIMDL_POSE_FADE_IN_AFTER_START;
			else if (!strcmp (str, "instant"))
				chan->fade_in.mode = LIMDL_POSE_FADE_IN_INSTANT;
		}
		if (liscr_args_gets_string (args, "fade_out_mode", &str))
		{
			if (!strcmp (str, "after end"))
				chan->fade_out.mode = LIMDL_POSE_FADE_OUT_AFTER_END;
			else if (!strcmp (str, "after end repeat"))
				chan->fade_out.mode = LIMDL_POSE_FADE_OUT_AFTER_END_REPEAT;
			else if (!strcmp (str, "before end"))
				chan->fade_out.mode = LIMDL_POSE_FADE_OUT_BEFORE_END;
			else if (!strcmp (str, "instant"))
				chan->fade_out.mode = LIMDL_POSE_FADE_OUT_INSTANT;
		}
	}

	/* Handle optional per-node priorities. */
	if (animation != NULL && liscr_args_gets_table (args, "node_priorities"))
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

	/* Handle optional per-node weights. */
	if (animation != NULL && liscr_args_gets_table (args, "node_weights"))
	{
		lua_pushnil (args->lua);
		while (lua_next (args->lua, -2) != 0)
		{
			if (lua_type (args->lua, -2) == LUA_TSTRING &&
				lua_type (args->lua, -1) == LUA_TNUMBER)
			{
				limdl_pose_channel_set_node_weight (chan,
					lua_tostring (args->lua, -2), lua_tonumber (args->lua, -1));
			}
			lua_pop (args->lua, 1);
		}
		lua_pop (args->lua, 1);
	}

	/* Set the animation. */
	liren_render_object_channel_animate (object->render, object->id, channel, keep, chan);
	liscr_args_seti_bool (args, 1);

	/* Free the pose channel arguments. */
	if (chan != NULL)
		limdl_pose_channel_free (chan);
}

static void RenderObject_animate_fade (LIScrArgs* args)
{
	int channel = 0;
	float time = LIMDL_POSE_FADE_AUTOMATIC;
	LIExtRenderObject* object;

	object = args->self;
	if (!liscr_args_gets_int (args, "channel", &channel))
		return;
	if (liscr_args_gets_float (args, "duration", &time))
		time = LIMAT_MAX (0, time);

	if (channel < 1 || channel > 255)
		return;
	channel--;

	liren_render_object_channel_fade (object->render, object->id, channel, time);
}

static void RenderObject_clear_animations (LIScrArgs* args)
{
	LIExtRenderObject* object;

	object = args->self;
	liren_render_object_clear_animations (object->render, object->id);
}

static void RenderObject_find_node (LIScrArgs* args)
{
	const char* name;
	const char* space = "local";
	LIMatTransform transform;
	LIExtRenderObject* object;

	object = args->self;
	if (!liscr_args_gets_string (args, "name", &name))
		return;
	liscr_args_gets_string (args, "space", &space);

	/* Get the transformation. */
	if (!liren_render_object_find_node (object->render, object->id, name, !strcmp (space, "world"), &transform))
		return;

	/* Return the transformation. */
	liscr_args_seti_vector (args, &transform.position);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

static void RenderObject_get_animation (LIScrArgs* args)
{
	int chan;
	LIAlgStrdicIter iter;
	LIExtRenderObject* object;
	LIMdlPoseChannel* channel;

	/* Check arguments. */
	object = args->self;
	if (!liscr_args_gets_int (args, "channel", &chan))
		return;
	chan--;
	channel = liren_render_object_channel_get_state (object->render, object->id, chan);
	if (channel == NULL)
		return;

	/* Set animation info. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_string (args, "animation", channel->animation->name);
	liscr_args_sets_int (args, "channel", chan + 1);
	liscr_args_sets_float (args, "fade_in", channel->fade_in.duration);
	liscr_args_sets_float (args, "fade_out", channel->fade_out.duration);
	liscr_args_sets_bool (args, "permanent", channel->repeats == -1);
	liscr_args_sets_float (args, "repeat_start", channel->repeat_start);
	liscr_args_sets_float (args, "time", channel->time);
	liscr_args_sets_float (args, "time_scale", channel->time_scale);
	liscr_args_sets_float (args, "priority", channel->priority_transform);
	liscr_args_sets_float (args, "priority_scale", channel->priority_scale);
	liscr_args_sets_float (args, "weight", channel->weight_transform);
	liscr_args_sets_float (args, "weight_scale", channel->weight_scale);

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

static void RenderObject_particle_animation (LIScrArgs* args)
{
	int loop = 1;
	float start = 0.0f;
	LIExtRenderObject* object;

	/* Get arguments. */
	liscr_args_gets_bool (args, "loop", &loop);
	liscr_args_gets_float (args, "time", &start);

	/* Set the animation mode. */
	object = args->self;
	liren_render_object_particle_animation (object->render, object->id, start, loop);
}

static void RenderObject_remove_model (LIScrArgs* args)
{
	LIExtRenderModel* model;
	LIExtRenderObject* object;
	LIScrData* value;

	if (liscr_args_geti_data (args, 0, LIEXT_SCRIPT_RENDER_MODEL, &value))
	{
		object = args->self;
		model = liscr_data_get_data (value);
		liren_render_object_remove_model (object->render, object->id, model->id);
	}
}

static void RenderObject_replace_model (LIScrArgs* args)
{
	LIExtRenderModel* model;
	LIExtRenderModel* model1;
	LIExtRenderObject* object;
	LIScrData* value;
	LIScrData* value1;

	if (liscr_args_geti_data (args, 0, LIEXT_SCRIPT_RENDER_MODEL, &value) &&
	    liscr_args_geti_data (args, 1, LIEXT_SCRIPT_RENDER_MODEL, &value1))
	{
		object = args->self;
		model = liscr_data_get_data (value);
		model1 = liscr_data_get_data (value1);
		liren_render_object_replace_model (object->render, object->id, model->id, model1->id);
	}
}

static void RenderObject_replace_texture (LIScrArgs* args)
{
	const char* name;
	LIExtRenderObject* self;
	LIImgImage* image;
	LIScrData* value;

	self = args->self;
	if (liscr_args_geti_string (args, 0, &name) &&
	    liscr_args_geti_data (args, 1, LIEXT_SCRIPT_IMAGE, &value))
	{
		image = liscr_data_get_data (value);
		liren_render_object_replace_texture (self->render, self->id, name,
			image->width, image->height, image->pixels);
	}
}

static void RenderObject_set_effect (LIScrArgs* args)
{
	float params[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const char* shader = NULL;
	LIExtRenderObject* object;

	/* Get arguments. */
	if (!liscr_args_geti_string (args, 0, &shader))
		liscr_args_gets_string (args, "shader", &shader);
	liscr_args_gets_floatv (args, "params", 4, params);

	/* Deform the mesh. */
	object = args->self;
	liren_render_object_set_effect (object->render, object->id, shader, params);
}

static void RenderObject_set_particle (LIScrArgs* args)
{
	const char* name;
	LIExtRenderObject* object;

	object = args->self;
	if (liscr_args_geti_string (args, 0, &name))
		liren_render_object_set_particle (object->render, object->id, name);
}

static void RenderObject_set_particle_emitting (LIScrArgs* args)
{
	int value;
	LIExtRenderObject* object;

	object = args->self;
	if (liscr_args_geti_bool (args, 0, &value))
		liren_render_object_set_particle_emitting (object->render, object->id, value);
}

static void RenderObject_set_render_distance (LIScrArgs* args)
{
	float value;
	LIExtRenderObject* object;

	object = args->self;
	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		liren_render_object_set_render_distance (object->render, object->id, value);
	else
		liren_render_object_set_render_distance (object->render, object->id, -1.0f);
}

static void RenderObject_get_render_loaded (LIScrArgs* args)
{
	LIExtRenderObject* object = args->self;

	if (liren_render_object_get_loaded (object->render, object->id))
		liscr_args_seti_bool (args, 1);
}

static void RenderObject_set_render_queue (LIScrArgs* args)
{
	const char* value;
	LIExtRenderObject* object;

	object = args->self;
	if (liscr_args_geti_string (args, 0, &value))
		liren_render_object_set_render_queue (object->render, object->id, value);
}

static void RenderObject_set_shadow_casting (LIScrArgs* args)
{
	int value;
	LIExtRenderObject* object;

	object = args->self;
	if (liscr_args_geti_bool (args, 0, &value))
		liren_render_object_set_shadow (object->render, object->id, value);
}

static void RenderObject_set_position (LIScrArgs* args)
{
	LIExtRenderObject* object;
	LIMatVector value;

	object = args->self;
	if (liscr_args_geti_vector (args, 0, &value))
	{
		object->transform.position = value;
		liren_render_object_set_transform (object->render, object->id, &object->transform);
	}
}

static void RenderObject_set_rotation (LIScrArgs* args)
{
	LIExtRenderObject* object;
	LIMatQuaternion value;

	object = args->self;
	if (liscr_args_geti_quaternion (args, 0, &value))
	{
		object->transform.rotation = value;
		liren_render_object_set_transform (object->render, object->id, &object->transform);
	}
}

static void RenderObject_set_visible (LIScrArgs* args)
{
	int value;
	LIExtRenderObject* object;

	object = args->self;
	if (liscr_args_geti_bool (args, 0, &value))
		liren_render_object_set_realized (object->render, object->id, value);
}

/*****************************************************************************/

void liext_script_render_object (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_new", RenderObject_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_add_model", RenderObject_add_model);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_add_texture_alias", RenderObject_add_texture_alias);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_animate", RenderObject_animate);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_animate_fade", RenderObject_animate_fade);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_clear_animations", RenderObject_clear_animations);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_find_node", RenderObject_find_node);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_get_animation", RenderObject_get_animation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_particle_animation", RenderObject_particle_animation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_remove_model", RenderObject_remove_model);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_replace_model", RenderObject_replace_model);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_replace_texture", RenderObject_replace_texture);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_set_effect", RenderObject_set_effect);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_set_particle", RenderObject_set_particle);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_set_particle_emitting", RenderObject_set_particle_emitting);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_set_position", RenderObject_set_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_set_render_distance", RenderObject_set_render_distance);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_get_render_loaded", RenderObject_get_render_loaded);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_set_render_queue", RenderObject_set_render_queue);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_set_rotation", RenderObject_set_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_set_shadow_casting", RenderObject_set_shadow_casting);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_OBJECT, "render_object_set_visible", RenderObject_set_visible);
}

/** @} */
/** @} */
