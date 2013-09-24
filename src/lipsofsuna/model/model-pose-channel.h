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

#ifndef __MODEL_POSE_CHANNEL_H__
#define __MODEL_POSE_CHANNEL_H__

#include "lipsofsuna/algorithm.h"
#include "model-animation.h"
#include "model-types.h"

enum _LIMdlPoseChannelState
{
	LIMDL_POSE_CHANNEL_STATE_INVALID,
	LIMDL_POSE_CHANNEL_STATE_PLAYING,
	LIMDL_POSE_CHANNEL_STATE_PAUSED
};

struct _LIMdlPoseChannel
{
	int additive;
	int fading;
	int state;
	int repeat;
	int repeats;
	float repeat_start;
	float repeat_end;
	float time;
	float time_scale;
	int priority_scale;
	int priority_transform;
	float weight_scale;
	float weight_transform;
	float fade_in;
	float fade_out;
	LIAlgStrdic* priorities;
	LIAlgStrdic* weights;
	LIMdlAnimation* animation;

	float fade_time;
	LIMdlPoseChannel* fade_prev;
	LIMdlPoseChannel* fade_next;
};

LIAPICALL (LIMdlPoseChannel*, limdl_pose_channel_new, (
	LIMdlAnimation* animation));

LIAPICALL (LIMdlPoseChannel*, limdl_pose_channel_new_copy, (
	LIMdlPoseChannel* channel));

LIAPICALL (void, limdl_pose_channel_free, (
	LIMdlPoseChannel* self));

LIAPICALL (int, limdl_pose_channel_play, (
	LIMdlPoseChannel* self,
	float             secs));

LIAPICALL (float, limdl_pose_channel_get_fading, (
	const LIMdlPoseChannel* self));

LIAPICALL (void, limdl_pose_channel_get_node_priority, (
	LIMdlPoseChannel* self,
	const char*       node,
	int*              scale,
	int*              transform));

LIAPICALL (int, limdl_pose_channel_set_node_priority, (
	LIMdlPoseChannel* self,
	const char*       node,
	int               value));

LIAPICALL (int, limdl_pose_channel_set_node_weight, (
	LIMdlPoseChannel* self,
	const char*       node,
	float             value));

LIAPICALL (void, limdl_pose_channel_get_weight, (
	const LIMdlPoseChannel* self,
	const char*             node,
	float*                  scale,
	float*                  transform));

#endif
