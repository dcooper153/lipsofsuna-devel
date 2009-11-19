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
#define LI_OBJECT_POSITION_ERROR 0.3f

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
	self = lieng_default_calls.lieng_object_new (module->engine, NULL, LIPHY_CONTROL_MODE_STATIC, id, NULL);
	if (self == NULL)
		return NULL;

	/* Allocate client data. */
	data = lisys_calloc (1, sizeof (licliObject));
	if (data == NULL)
		goto error;
	data->module = module;

	/* Extend engine object. */
	lieng_object_set_userdata (self, LIENG_DATA_CLIENT, data);
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
		lisys_free (data);
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
	liSpeech* speech;

	/* Call base. */
	lieng_default_calls.lieng_object_update (self, secs);

	data = LICLI_OBJECT (self);
	if (data == NULL)
		return;

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

	pose = self->pose;
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

/** @} */
/** @} */
