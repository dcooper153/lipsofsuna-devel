/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlPose Pose
 * @{
 */

#ifndef __MODEL_POSE_H__
#define __MODEL_POSE_H__

#include <algorithm/lips-algorithm.h>
#include "model.h"
#include "model-animation.h"
#include "model-bone.h"
#include "model-types.h"
#include "model-vertex.h"

#define LIMDL_POSE_FADE_AUTOMATIC -1.0f

enum _limdlPoseChannelState
{
	LIMDL_POSE_CHANNEL_STATE_INVALID,
	LIMDL_POSE_CHANNEL_STATE_PLAYING,
	LIMDL_POSE_CHANNEL_STATE_PAUSED,
};

struct _limdlPoseChannel
{
	int state;
	int repeat;
	int repeats;
	float time;
	float priority;
	char* animation_name;
	limdlAnimation* animation;
};

struct _limdlPoseFade
{
	float rate;
	float time;
	float weight;
	char* animation_name;
	limdlPoseFade* prev;
	limdlPoseFade* next;
	limdlAnimation* animation;
};

struct _limdlPose
{
	limdlModel* model;
	lialgU32dic* channels;
	limdlPoseFade* fades;
	struct
	{
		int count;
		limdlWeightGroup* array;
	} groups;
	struct
	{
		int count;
		limdlNode** array;
	} nodes;
};

#ifdef __cplusplus
extern "C" {
#endif

limdlPose*
limdl_pose_new ();

void
limdl_pose_free (limdlPose* self);

void
limdl_pose_destroy_channel (limdlPose* self,
                            int        channel);

void
limdl_pose_fade_channel (limdlPose* self,
                         int        channel,
                         float      rate);

limdlNode*
limdl_pose_find_node (const limdlPose* self,
                      const char*      name);

void
limdl_pose_update (limdlPose* self,
                   float      secs);

void
limdl_pose_transform (limdlPose*   self,
                      limdlVertex* vertices);

limdlAnimation*
limdl_pose_get_channel_animation (const limdlPose* self,
                                  int              channel);

void
limdl_pose_set_channel_animation (limdlPose*  self,
                                  int         channel,
                                  const char* animation);

const char*
limdl_pose_get_channel_name (const limdlPose* self,
                             int              channel);

float
limdl_pose_get_channel_position (const limdlPose* self,
                                 int              channel);

void
limdl_pose_set_channel_priority (limdlPose* self,
                                 int        channel,
                                 float      value);

int
limdl_pose_get_channel_repeats (const limdlPose* self,
                                int              channel);

void
limdl_pose_set_channel_repeats (limdlPose* self,
                                int        channel,
                                int        value);

limdlPoseChannelState
limdl_pose_get_channel_state (const limdlPose* self,
                              int              channel);

void
limdl_pose_set_channel_state (limdlPose*            self,
                              int                   channel,
                              limdlPoseChannelState value);

int
limdl_pose_set_model (limdlPose*  self,
                      limdlModel* model);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
