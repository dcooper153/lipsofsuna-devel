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
 * \addtogroup limdl Model
 * @{
 * \addtogroup LIMdlPose Pose
 * @{
 */

#ifndef __MODEL_POSE_H__
#define __MODEL_POSE_H__

#include <lipsofsuna/algorithm.h>
#include "model.h"
#include "model-animation.h"
#include "model-bone.h"
#include "model-types.h"
#include "model-vertex.h"

#define LIMDL_POSE_FADE_AUTOMATIC -1.0f

enum _LIMdlPoseChannelState
{
	LIMDL_POSE_CHANNEL_STATE_INVALID,
	LIMDL_POSE_CHANNEL_STATE_PLAYING,
	LIMDL_POSE_CHANNEL_STATE_PAUSED,
};

struct _LIMdlPoseChannel
{
	int state;
	int repeat;
	int repeats;
	float time;
	float priority;
	char* animation_name;
	LIMdlAnimation* animation;
};

struct _LIMdlPoseFade
{
	float rate;
	float time;
	float weight;
	char* animation_name;
	LIMdlPoseFade* prev;
	LIMdlPoseFade* next;
	LIMdlAnimation* animation;
};

struct _LIMdlPose
{
	LIMdlModel* model;
	LIAlgU32dic* channels;
	LIMdlPoseFade* fades;
	struct
	{
		int count;
		LIMdlWeightGroup* array;
	} groups;
	struct
	{
		int count;
		LIMdlNode** array;
	} nodes;
};

#ifdef __cplusplus
extern "C" {
#endif

LIMdlPose*
limdl_pose_new ();

void
limdl_pose_free (LIMdlPose* self);

void
limdl_pose_destroy_channel (LIMdlPose* self,
                            int        channel);

void
limdl_pose_fade_channel (LIMdlPose* self,
                         int        channel,
                         float      rate);

LIMdlNode*
limdl_pose_find_node (const LIMdlPose* self,
                      const char*      name);

void
limdl_pose_update (LIMdlPose* self,
                   float      secs);

void
limdl_pose_transform (LIMdlPose*   self,
                      LIMdlVertex* vertices);

LIMdlAnimation*
limdl_pose_get_channel_animation (const LIMdlPose* self,
                                  int              channel);

void
limdl_pose_set_channel_animation (LIMdlPose*  self,
                                  int         channel,
                                  const char* animation);

const char*
limdl_pose_get_channel_name (const LIMdlPose* self,
                             int              channel);

float
limdl_pose_get_channel_position (const LIMdlPose* self,
                                 int              channel);

void
limdl_pose_set_channel_position (LIMdlPose* self,
                                 int        channel,
                                 float      value);

float
limdl_pose_get_channel_priority (const LIMdlPose* self,
                                 int              channel);

void
limdl_pose_set_channel_priority (LIMdlPose* self,
                                 int        channel,
                                 float      value);

int
limdl_pose_get_channel_repeats (const LIMdlPose* self,
                                int              channel);

void
limdl_pose_set_channel_repeats (LIMdlPose* self,
                                int        channel,
                                int        value);

LIMdlPoseChannelState
limdl_pose_get_channel_state (const LIMdlPose* self,
                              int              channel);

void
limdl_pose_set_channel_state (LIMdlPose*            self,
                              int                   channel,
                              LIMdlPoseChannelState value);

int
limdl_pose_set_model (LIMdlPose*  self,
                      LIMdlModel* model);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
