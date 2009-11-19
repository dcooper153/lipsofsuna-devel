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
 * \addtogroup licliObject Object
 * @{
 */

#include "client.h"
#include "client-object.h"
#include "client-window.h"

#define LICLI_OBJECT_POSITION_SMOOTHING 0.5f
#define LICLI_OBJECT_ROTATION_SMOOTHING 0.5f

/**
 * \brief Creates a new object.
 *
 * \param module Module.
 * \param id The id number of the object.
 * \param flags Mode flags.
 * \return A new object or NULL.
 */
liengObject*
licli_object_new (licliModule* module,
                  uint32_t     id,
                  int          flags)
{
	liengObject* self;
	liscrScript* script = module->script;

	/* Allocate engine data. */
	self = lieng_default_calls.lieng_object_new (module->engine, NULL, LIPHY_CONTROL_MODE_STATIC, id, NULL);
	if (self == NULL)
		return NULL;

	/* Extend engine object. */
	lieng_object_set_smoothing (self, LICLI_OBJECT_POSITION_SMOOTHING, LICLI_OBJECT_ROTATION_SMOOTHING);
	lieng_object_set_userdata (self, LIENG_DATA_CLIENT, (void*) -1);
//	lieng_object_set_flags (self, flags);
	if (flags & LINET_OBJECT_FLAG_DYNAMIC)
	{
		lieng_object_set_collision_group (self, LICLI_PHYSICS_GROUP_OBJECTS);
		lieng_object_set_collision_mask (self,
			LICLI_PHYSICS_GROUP_OBJECTS | LIPHY_GROUP_STATICS);
	}
	else
	{
		lieng_object_set_collision_group (self, LIPHY_GROUP_STATICS);
		lieng_object_set_collision_mask (self,
			LICLI_PHYSICS_GROUP_CAMERA | LICLI_PHYSICS_GROUP_OBJECTS);
	}

	/* Allocate script data. */
	self->script = liscr_data_new (script, self, LICOM_SCRIPT_OBJECT);
	if (self->script == NULL)
		goto error;

	return self;

error:
	lieng_object_free (self);
	return NULL;
}

void
licli_object_set_animation (liengObject* self,
                            int          value,
                            int          channel,
                            int          permanent,
                            float        priority)
{
	int chan;
	liengAnimation* animation;
	limdlPose* pose;

	pose = self->pose;
	chan = channel + 65536;

	if (value == LINET_INVALID_ANIMATION)
	{
		limdl_pose_fade_channel (pose, chan, LIMDL_POSE_FADE_AUTOMATIC);
		return;
	}

	animation = lieng_engine_find_animation_by_code (self->engine, value);
	if (animation == NULL)
		return;

	limdl_pose_fade_channel (pose, chan, LIMDL_POSE_FADE_AUTOMATIC);
	limdl_pose_set_channel_animation (pose, chan, animation->name);
	limdl_pose_set_channel_repeats (pose, chan, permanent? -1 : 1);
	limdl_pose_set_channel_priority (pose, chan, priority);
	limdl_pose_set_channel_state (pose, chan, LIMDL_POSE_CHANNEL_STATE_PLAYING);
}

/** @} */
/** @} */
