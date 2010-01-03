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
 * \addtogroup LICliObject Object
 * @{
 */

#include "client.h"
#include "client-object.h"
#include "client-window.h"

void
licli_object_set_animation (LIEngObject* self,
                            int          value,
                            int          channel,
                            int          permanent,
                            float        priority)
{
	int chan;
	LIEngAnimation* animation;
	LIMdlPose* pose;

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

void
licli_object_set_flags (LIEngObject* self,
                        int          value)
{
	if (value & LINET_OBJECT_FLAG_DYNAMIC)
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
}

/** @} */
/** @} */
