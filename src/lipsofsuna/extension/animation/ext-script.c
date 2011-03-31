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
 * \addtogroup LIExtAnimation Animation
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "core/animation"
 * --- Animation playback for objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * --- Sets or clears an animation.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>animation: Animation name.</li>
 * --   <li>channel: Channel number.</li>
 * --   <li>fade_in: Fade in duration in seconds.</li>
 * --   <li>fade_out: Fade out duration in seconds.</li>
 * --   <li>weight: Blending weight.</li>
 * --   <li>time: Starting time.</li>
 * --   <li>permanent: True if should keep repeating.</li></ul>
 * --   <li>repeat_start: Starting time when repeating.</li>
 * -- @return True if started a new animation.
 * function Object.animate(self, args)
 */
static void Object_animate (LIScrArgs* args)
{
	int ret;
	int repeat = 0;
	int repeat_start = 0;
	int channel = -1;
	float fade_in = 0.0f;
	float fade_out = 0.0f;
	float weight = 1.0f;
	float weight_scale = 0.0f;
	float time = 0.0f;
	const char* animation = NULL;

	liscr_args_gets_string (args, "animation", &animation);
	liscr_args_gets_int (args, "channel", &channel);
	liscr_args_gets_float (args, "fade_in", &fade_in);
	liscr_args_gets_float (args, "fade_out", &fade_out);
	liscr_args_gets_float (args, "weight", &weight);
	liscr_args_gets_float (args, "weight_scale", &weight_scale);
	liscr_args_gets_float (args, "time", &time);
	liscr_args_gets_bool (args, "permanent", &repeat);
	liscr_args_gets_int (args, "repeat_start", &repeat_start);
	if (channel < 1 || channel > 255)
		channel = -1;
	else
		channel--;
	repeat_start = LIMAT_MAX (0, repeat_start);
	ret = lieng_object_animate (args->self, channel, animation, repeat, repeat_start, weight_scale, weight, time, fade_in, fade_out);
	liscr_args_seti_bool (args, ret);
}

/* @luadoc
 * --- Fades out an animation channel.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>channel: Channel number.</li>
 * --   <li>duration: Fade duration in seconds.</li></ul>
 * function Object.animate_fade(self, args)
 */
static void Object_animate_fade (LIScrArgs* args)
{
	int channel = 0;
	float time = LIMDL_POSE_FADE_AUTOMATIC;
	LIEngObject* object;

	if (!liscr_args_gets_int (args, "channel", &channel))
		return;
	if (liscr_args_gets_float (args, "duration", &time))
		time = LIMAT_MAX (0, time);

	if (channel < 1 || channel > 255)
		return;
	channel--;

	object = args->self;
	if (object->pose != NULL)
		limdl_pose_fade_channel (object->pose, channel, time);
}

/* @luadoc
 * --- Edits the pose of a node.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>channel: Channel number.</li>
 * --   <li>frame: Frame number.</li>
 * --   <li>node: Node name. (required)</li>
 * --   <li>position: Position change relative to rest pose.</li>
 * --   <li>rotation: Rotation change relative to rest pose.</li>
 * --   <li>scale: Scale factor.</li></ul>
 * function Object.edit_pose(self, args)
 */
static void Object_edit_pose (LIScrArgs* args)
{
	int frame = 0;
	int channel = 0;
	float scale = 1.0f;
	const char* node = NULL;
	LIMatTransform transform = limat_transform_identity ();
	LIEngObject* self = args->self;

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
	liscr_args_gets_quaternion (args, "rotation", &transform.rotation);
	liscr_args_gets_vector (args, "position", &transform.position);
	liscr_args_gets_float (args, "scale", &scale);
	transform.rotation = limat_quaternion_normalize (transform.rotation);

	limdl_pose_set_channel_transform (self->pose, channel, frame, node, scale, &transform);
}

/* @luadoc
 * --- Finds a bone or an anchor by name.
 * --
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>name: Node name. (required)</li>
 * --   <li>space: Coordinate space. ("local"/"world")</li></ul>
 * -- @return Position and rotation, or nil if not found.
 * function Object.find_node(self, args)
 */
static void Object_find_node (LIScrArgs* args)
{
	float scale;
	const char* name;
	const char* space = "local";
	LIMatTransform transform;
	LIMatTransform transform1;
	LIMdlNode* node;
	LIEngObject* self;

	if (!liscr_args_gets_string (args, "name", &name))
		return;
	liscr_args_gets_string (args, "space", &space);

	/* Find the node. */
	self = args->self;
	node = limdl_pose_find_node (self->pose, name);
	if (node == NULL)
		return;

	/* Get the transformation. */
	if (!strcmp (space, "world"))
	{
		limdl_node_get_world_transform (node, &scale, &transform);
		lieng_object_get_transform (self, &transform1);
		transform = limat_transform_multiply (transform1, transform);
	}
	else
		limdl_node_get_world_transform (node, &scale, &transform);

	/* Return the transformation. */
	liscr_args_seti_vector (args, &transform.position);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

/* @luadoc
 * --- Gets animation information for the given animation channel.<br/>
 * -- If an animation is looping in the channel, a table containing the fields
 * -- animation, time, and weight is returned.
 * -- @param self Server class.
 * -- @param args Arguments.<ul>
 * --   <li>channel: Channel number. (required)</li></ul>
 * -- @return Animation info table or nil.
 * function Object.get_animation(self, args)
 */
static void Object_get_animation (LIScrArgs* args)
{
	int chan;
	LIEngObject* object;
	LIMdlAnimation* anim;

	/* Check arguments. */
	if (!liscr_args_gets_int (args, "channel", &chan))
		return;
	chan--;
	object = args->self;
	anim = limdl_pose_get_channel_animation (object->pose, chan);
	if (anim == NULL)
		return;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_string (args, "animation", anim->name);
	liscr_args_sets_bool (args, "repeat_start", limdl_pose_get_channel_repeat_start (object->pose, chan));
	liscr_args_sets_float (args, "time", limdl_pose_get_channel_position (object->pose, chan));
	liscr_args_sets_float (args, "weight", limdl_pose_get_channel_priority_transform (object->pose, chan));
	liscr_args_sets_float (args, "weight_scale", limdl_pose_get_channel_priority_scale (object->pose, chan));
}

/* @luadoc
 * --- Gets the list of active animations.
 * -- @param self Server class.
 * -- @return Animation list.
 * function Object.get_animations(self)
 */
static void Object_get_animations (LIScrArgs* args)
{
	LIAlgU32dicIter iter;
	LIEngObject* object;
	LIMdlPoseChannel* channel;

	object = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, object->pose->channels)
	{
		channel = iter.value;
		if (channel->repeats == -1)
			liscr_args_setf_string (args, iter.key + 1, channel->animation->name);
	}
}

/* @luadoc
 * --- Updates the animations of the object.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>secs: Tick length.</li></ul>
 * function Object.update_animations(self, args)
 */
static void Object_update_animations (LIScrArgs* args)
{
	float secs = 1.0f;
	LIEngObject* self;

	self = args->self;
	liscr_args_gets_float (args, "secs", &secs);
	if (self->pose != NULL)
		limdl_pose_update (self->pose, secs);
}

/*****************************************************************************/

void liext_script_object_animation (
	LIScrClass* self,
	void*       data)
{
	liscr_class_insert_mfunc (self, "animate", Object_animate);
	liscr_class_insert_mfunc (self, "animate_fade", Object_animate_fade);
	liscr_class_insert_mfunc (self, "edit_pose", Object_edit_pose);
	liscr_class_insert_mfunc (self, "find_node", Object_find_node);
	liscr_class_insert_mfunc (self, "get_animation", Object_get_animation);
	liscr_class_insert_mfunc (self, "get_animations", Object_get_animations);
	liscr_class_insert_mfunc (self, "update_animations", Object_update_animations);
}

/** @} */
/** @} */
