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
#include "client-speech.h"
#include "client-window.h"

#define LI_OBJECT_SPEED_UNIT (0.01f / LI_MAP_SCALE)
#define LI_OBJECT_SPEED_MODIFIER 1.0f
#define LI_OBJECT_ROTATION_MODIFIER 1.5f
#define LI_OBJECT_DIRECTION_CORRECTION 0.5f
#define LI_OBJECT_POSITION_ERROR 0.3f
#define LI_OBJECT_POSITION_CORRECTION 0.5f

enum
{
	LI_OBJECT_CHANNEL_MOVE,
	LI_OBJECT_CHANNEL_TURN,
	LI_OBJECT_CHANNEL_ACTION,
	LI_OBJECT_CHANNEL_COUNT,
};

/*****************************************************************************/

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
	licliObject* data;
	liengObject* self;
	liscrScript* script = module->script;

	/* Allocate engine data. */
	self = lieng_default_calls.lieng_object_new (module->engine, NULL,
		LIPHY_SHAPE_MODE_CONVEX, LIPHY_CONTROL_MODE_STATIC, id, NULL);
	if (self == NULL)
		return NULL;

	/* Allocate client data. */
	data = calloc (1, sizeof (licliObject));
	if (data == NULL)
		goto error;
	data->module = module;
	data->curr.transform = limat_transform_identity ();
	data->prev.transform = limat_transform_identity ();

	/* Extend engine object. */
	lieng_object_set_userdata (self, LIENG_DATA_CLIENT, data);
//	lieng_object_set_flags (self, flags);
	if (flags & LINET_OBJECT_FLAG_DYNAMIC)
	{
		lieng_object_set_collision_group (self, LICLI_PHYSICS_GROUP_OBJECTS);
		lieng_object_set_collision_mask (self, LICLI_PHYSICS_GROUP_OBJECTS | LICLI_PHYSICS_GROUP_STATICS);
	}
	else
	{
		lieng_object_set_collision_group (self, LICLI_PHYSICS_GROUP_STATICS);
		lieng_object_set_collision_mask (self, LICLI_PHYSICS_GROUP_CAMERA | LICLI_PHYSICS_GROUP_OBJECTS | LICLI_PHYSICS_GROUP_STATICS);
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

/**
 * \brief Frees the object.
 *
 * \param self An object.
 */
void
licli_object_free (liengObject* self)
{
	lialgList* ptr;
	licliObject* data = LICLI_OBJECT (self);

	/* Free client data. */
	if (data != NULL)
	{
		/* Free speech. */
		for (ptr = data->speech ; ptr != NULL ; ptr = ptr->next)
			li_speech_free (ptr->data);
		lialg_list_free (data->speech);

		/* Free sounds. */
#ifndef LI_DISABLE_SOUND
		for (ptr = data->sounds ; ptr != NULL ; ptr = ptr->next)
			lisnd_source_free (ptr->data);
		lialg_list_free (data->sounds);
#endif

		free (data);
	}

	/* Call base. */
	lieng_default_calls.lieng_object_free (self);
}

/**
 * \brief Updates the orientation of the object.
 *
 * \param self An object.
 * \param secs The number of seconds since the last update.
 */
void
licli_object_update (liengObject* self,
                     float        secs)
{
	float t;
	lialgList* ptr;
	lialgList* next;
	licliObject* data;
	limatAabb aabb;
	liSpeech* speech;
#ifndef LI_DISABLE_SOUND
	lisndSource* source;
#endif

	/* Call base. */
	lieng_default_calls.lieng_object_update (self, secs);

	data = LICLI_OBJECT (self);
	if (data == NULL)
		return;

	/* Get bounds. */
	lieng_object_get_bounds (self, &aabb);
	limat_aabb_average (&aabb);

	/* Interpolate position. */
	data->prev.transform.position = limat_vector_lerp (
		data->curr.transform.position, data->prev.transform.position,
		0.5f * LI_OBJECT_POSITION_CORRECTION);

	/* Interpolate orientation. */
	data->prev.transform.rotation = limat_quaternion_get_nearest (
		data->prev.transform.rotation, data->curr.transform.rotation);
	data->prev.transform.rotation = limat_quaternion_nlerp (
		data->curr.transform.rotation, data->prev.transform.rotation,
		0.5f * LI_OBJECT_DIRECTION_CORRECTION);
	lieng_default_calls.lieng_object_set_transform (self, &data->prev.transform);

	/* Apply movement. */
//	liwrl_object_update (LIWRL_OBJECT (self), client->map, &aabb, secs);
/*	if (liwrl_object_get_dirty (LIWRL_OBJECT (self)))
	{
		LIWRL_OBJECT (self)->flags_prev = LIWRL_OBJECT (self)->flags;
		LIWRL_OBJECT (self)->position_prev = LIWRL_OBJECT (self)->position;
		LIWRL_OBJECT (self)->velocity_prev = LIWRL_OBJECT (self)->velocity;
		LIWRL_OBJECT (self)->direction_prev = LIWRL_OBJECT (self)->direction;
	}*/

	/* Update speech. */
	for (ptr = data->speech ; ptr != NULL ; ptr = next)
	{
		next = ptr->next;
		speech = ptr->data;
		speech->timer += secs;
		if (speech->timer > LI_SPEECH_TIMEOUT)
		{
			li_speech_free (speech);
			lialg_list_remove (&data->speech, ptr);
		}
		else
		{
			t = speech->timer / LI_SPEECH_TIMEOUT;
			if (t < 0.5)
				speech->alpha = 1.0f;
			else
				speech->alpha = 1.0f - powf (2.0f * t - 0.5, 4);
		}
	}

	/* Update sound effects. */
#ifndef LI_DISABLE_SOUND
	for (ptr = data->sounds ; ptr != NULL ; ptr = next)
	{
		next = ptr->next;
		source = ptr->data;
		lisnd_source_set_position (source, &data->curr.transform.position);
		lisnd_source_set_velocity (source, &data->curr.velocity);
		if (!lisnd_source_update (source))
		{
			lisnd_source_free (source);
			lialg_list_remove (&data->sounds, ptr);
		}
	}
#endif
}

/**
 * \brief Sets the object to its target state.
 *
 * \param self Object.
 */
void
licli_object_warp (liengObject* self)
{
	licliObject* data = LICLI_OBJECT (self);

	data->prev.transform = data->curr.transform;
	data->prev.velocity = data->curr.velocity;
}

void
licli_object_set_animation (liengObject* self,
                            int          value,
                            int          channel,
                            int          permanent,
                            float        priority)
{
	int chan;
	licliObject* data = LICLI_OBJECT (self);
	liengAnimation* animation;
	limdlPose* pose;

	pose = self->render->pose.pose;
	chan = channel + 65536;

	if (value == LINET_INVALID_ANIMATION)
	{
		limdl_pose_fade_channel (pose, chan, LIMDL_POSE_FADE_AUTOMATIC);
		return;
	}

	animation = lieng_engine_find_animation_by_code (data->module->engine, value);
	if (animation == NULL)
		return;

	limdl_pose_fade_channel (pose, chan, LIMDL_POSE_FADE_AUTOMATIC);
	limdl_pose_set_channel_animation (pose, chan, animation->name);
	limdl_pose_set_channel_repeats (pose, chan, permanent? -1 : 1);
	limdl_pose_set_channel_priority (pose, chan, priority);
	limdl_pose_set_channel_state (pose, chan, LIMDL_POSE_CHANNEL_STATE_PLAYING);
}

/**
 * \brief Gets the bounding box size of the object.
 *
 * \param self Object.
 * \param bounds Return location for the bounding box.
 */
void
licli_object_get_bounds (const liengObject* self,
                         limatAabb*         bounds)
{
	lieng_object_get_bounds (self, bounds);
}

void
licli_object_get_center (const liengObject* self,
                         limatVector*       center)
{
	licliObject* data = LICLI_OBJECT (self);

	*center = data->curr.transform.position;
	/* FIXME: Take bounding box into account. */
}

/**
 * \brief Sets the target direction of the object.
 *
 * \param self Object.
 * \param direction Quaternion.
 */
void
licli_object_set_direction (liengObject*           self,
                            const limatQuaternion* direction)
{
	licliObject* data = LICLI_OBJECT (self);

	data->curr.transform.rotation = *direction;
}

void
licli_object_set_effect (liengObject* self,
                         uint32_t     id,
                         int          flags)
{
#ifndef LI_DISABLE_SOUND
	licliObject* data = LICLI_OBJECT (self);
	lisndSample* sample;
	lisndSource* source;

	/* Set sound effect. */
	if (data->module->client->sound != NULL)
	{
		sample = licli_module_find_sample_by_id (data->module, id);
		if (sample != NULL)
		{
			source = lisnd_source_new_with_sample (data->module->client->sound, sample);
			if (source != NULL)
			{
				if (lialg_list_prepend (&data->sounds, source))
				{
					lisnd_source_set_position (source, &data->curr.transform.position);
					lisnd_source_set_velocity (source, &data->curr.velocity);
					if (flags & LI_EFFECT_REPEAT)
						lisnd_source_set_looping (source, 1);
					lisnd_source_set_playing (source, 1);
				}
				else
					lisnd_source_free (source);
			}
		}
	}
#endif
}

int
licli_object_set_realized (liengObject* self,
                           int          value)
{
	if (value == lieng_object_get_realized (self))
		return 1;
	if (value)
	{
		/* Call base. */
		if (!lieng_default_calls.lieng_object_set_realized (self, 1))
			return 0;

		/* Invoke callbacks. */
		lieng_engine_call (self->engine, LICLI_CALLBACK_VISIBILITY, self, 1);
	}
	else
	{
		/* Invoke callbacks. */
		lieng_engine_call (self->engine, LICLI_CALLBACK_VISIBILITY, self, 0);

		/* Call base. */
		if (!lieng_default_calls.lieng_object_set_realized (self, 0))
			return 0;
	}

	return 1;
}

/**
 * \brief Sets the latest chat message of the object.
 *
 * \param self An object.
 * \param message A string.
 */
void
licli_object_set_speech (liengObject* self,
                         const char*  message)
{
	licliObject* data = LICLI_OBJECT (self);
	liSpeech* speech;

	speech = li_speech_new (data->module, message);
	if (speech == NULL)
		return;
	if (!lialg_list_prepend (&data->speech, speech))
		li_speech_free (speech);
}

/**
 * \brief Sets the target transformation of the object.
 *
 * \param self Object.
 * \param value Transformation.
 * \return Nonzero on success.
 */
int
licli_object_set_transform (liengObject*          self,
                            const limatTransform* value)
{
	licliObject* data = LICLI_OBJECT (self);

	if (data == NULL)
		lieng_default_calls.lieng_object_set_transform (self, value);
	else
		data->curr.transform = *value;

	return 1;
}

void
licli_object_get_transform_target (liengObject*    self,
                                   limatTransform* value)
{
	licliObject* data = LICLI_OBJECT (self);

	*value = data->curr.transform;
}

/**
 * \brief Sets the target velocity of the object.
 *
 * \param self Object.
 * \param value Velocity vector.
 * \return Nonzero on success.
 */
int
licli_object_set_velocity (liengObject*       self,
                           const limatVector* value)
{
	licliObject* data = LICLI_OBJECT (self);

	if (data != NULL)
	{
		data->curr.velocity = *value;
		/* FIXME: No interpolation. */
		data->prev.velocity = *value;
	}
	else
		return lieng_default_calls.lieng_object_set_velocity (self, value);

	return 1;
}

/** @} */
/** @} */
