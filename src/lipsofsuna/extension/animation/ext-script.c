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
 * \addtogroup LIExtAnimation Animation
 * @{
 */

#include "ext-module.h"

static void Animation_new (LIScrArgs* args)
{
	const char* name;
	LIMdlAnimation* self;
	LIScrData* data;

	/* Get arguments. */
	if (!liscr_args_geti_string (args, 0, &name))
		return;

	/* Allocate the animation. */
	self = limdl_animation_new ();
	if (self == NULL)
		return;
	self->name = lisys_string_dup (name);
	if (self->name == NULL)
	{
		lisys_free (self);
		return;
	}

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_ANIMATION, limdl_animation_free);
	if (data == NULL)
	{
		limdl_animation_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Animation_copy (LIScrArgs* args)
{
	LIMdlAnimation* self;
	LIMdlAnimation* copy;
	LIScrData* data;

	/* Get arguments. */
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_ANIMATION, &data))
		return;
	copy = liscr_data_get_data (data);

	/* Allocate the animation. */
	self = limdl_animation_new_copy (copy);
	if (self == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_ANIMATION, limdl_animation_free);
	if (data == NULL)
	{
		limdl_animation_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Animation_load (LIScrArgs* args)
{
	char* file;
	const char* path;
	const char* name;
	LIMdlAnimation* self;
	LIExtAnimationModule* module;
	LIScrData* data;

	/* Get arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_ANIMATION);
	if (!liscr_args_geti_string (args, 0, &name))
		return;

	/* Create the path. */
	file = lisys_string_concat (name, ".lani");
	if (file == NULL)
		return;
	path = lipth_paths_find_file (module->program->paths, file);
	lisys_free (file);
	if (path == NULL)
		return;

	/* Allocate the animation. */
	self = limdl_animation_new_from_file (path);
	if (self == NULL)
	{
		lisys_error_report ();
		return;
	}

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_ANIMATION, limdl_animation_free);
	if (data == NULL)
	{
		limdl_animation_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Animation_get_duration (LIScrArgs* args)
{
	LIMdlAnimation* self;

	self = args->self;
	liscr_args_seti_float (args, limdl_animation_get_duration (self));
}

static void Animation_get_frames (LIScrArgs* args)
{
	LIMdlAnimation* self;

	self = args->self;
	liscr_args_seti_int (args, limdl_animation_get_length (self));
}

static void Animation_get_name (LIScrArgs* args)
{
	LIMdlAnimation* self;

	self = args->self;
	liscr_args_seti_string (args, self->name);
}

static void Animation_set_transform (LIScrArgs* args)
{
	int frame = 0;
	float scale = 1.0f;
	const char* node = NULL;
	LIMatTransform transform;
	LIMdlAnimation* self;

	self = args->self;

	/* Get arguments. */
	if (liscr_args_gets_int (args, "frame", &frame))
	{
		frame--;
		if (frame < 0)
			return;
	}
	if (!liscr_args_gets_string (args, "node", &node))
		return;
	transform = limat_transform_identity ();
	liscr_args_gets_quaternion (args, "rotation", &transform.rotation);
	liscr_args_gets_vector (args, "position", &transform.position);
	liscr_args_gets_float (args, "scale", &scale);
	transform.rotation = limat_quaternion_normalize (transform.rotation);

	/* Create the channel and frame. */
	if (!limdl_animation_insert_channel (self, node))
		return;
	if (limdl_animation_get_length (self) <= frame)
	{
		if (!limdl_animation_set_length (self, frame + 1))
			return;
	}

	/* Set the node transformation of the frame. */
	limdl_animation_set_transform (self, node, frame, scale, &transform);
}

/*****************************************************************************/

void liext_script_animation (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_ANIMATION, "animation_new", Animation_new);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_ANIMATION, "animation_copy", Animation_copy);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_ANIMATION, "animation_load", Animation_load);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_ANIMATION, "animation_get_duration", Animation_get_duration);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_ANIMATION, "animation_get_frames", Animation_get_frames);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_ANIMATION, "animation_get_name", Animation_get_name);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_ANIMATION, "animation_set_transform", Animation_set_transform);
}

/** @} */
/** @} */
