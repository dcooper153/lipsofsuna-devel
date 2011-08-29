/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#ifndef __RENDER_INTERNAL_MESSAGE_H__
#define __RENDER_INTERNAL_MESSAGE_H__

#include "lipsofsuna/system.h"
#include "render-types.h"

enum
{
	/* Render */
	LIREN_MESSAGE_RENDER_LOAD_FONT,
	LIREN_MESSAGE_RENDER_LOAD_IMAGE,
	LIREN_MESSAGE_RENDER_MEASURE_TEXT,
	LIREN_MESSAGE_RENDER_SCREENSHOT,
	LIREN_MESSAGE_RENDER_UPDATE,
	LIREN_MESSAGE_RENDER_GET_ANISOTROPY,
	LIREN_MESSAGE_RENDER_SET_ANISOTROPY,
	LIREN_MESSAGE_RENDER_GET_IMAGE_SIZE,
	LIREN_MESSAGE_RENDER_SET_VIDEOMODE,

	/* Light */
	/* TODO */

	/* Model */
	LIREN_MESSAGE_MODEL_NEW,
	LIREN_MESSAGE_MODEL_FREE,
	LIREN_MESSAGE_MODEL_SET_MODEL,

	/* Object */
	LIREN_MESSAGE_OBJECT_NEW,
	LIREN_MESSAGE_OBJECT_FREE,
	LIREN_MESSAGE_OBJECT_DEFORM,
	LIREN_MESSAGE_OBJECT_PARTICLE_ANIMATION,
	LIREN_MESSAGE_OBJECT_SET_EFFECT,
	LIREN_MESSAGE_OBJECT_SET_MODEL,
	LIREN_MESSAGE_OBJECT_SET_POSE,
	LIREN_MESSAGE_OBJECT_SET_REALIZED,
	LIREN_MESSAGE_OBJECT_SET_TRANSFORM,

	/* Overlay */
	/* TODO */

	/* Shader */
	/* TODO */
};

struct _LIRenMessage
{
	int type;
	LIRenMessage* next;
	LISysMutex* mutex_caller;
	union
	{
		/* Render */
		struct
		{
			char* name;
			char* file;
			int size;
		} render_load_font;
		struct
		{
			char* name;
		} render_load_image;
		struct
		{
			char* font;
			char* text;
			int width_limit;
			int* result_width;
			int* result_height;
		} render_measure_text;
		struct
		{
			SDL_Surface** result;
		} render_screenshot;
		struct
		{
			float secs;
		} render_update;
		struct
		{
			int* result;
		} render_get_anisotropy;
		struct
		{
			int value;
		} render_set_anisotropy;
		struct
		{
			char* name;
			int* result;
		} render_get_image_size;
		struct
		{
			LIRenVideomode* mode;
		} render_set_videomode;

		/* Light */
		/* TODO */

		/* Model */
		struct
		{
			LIMdlModel* model;
			int id;
			int* result_id;
		} model_new;
		struct
		{
			int id;
		} model_free;
		struct
		{
			int id;
			LIMdlModel* model;
		} model_set_model;

		/* Object */
		struct
		{
			int id;
			int* result_id;
		} object_new;
		struct
		{
			int id;
		} object_free;
		struct
		{
			int id;
		} object_deform;
		struct
		{
			int id;
			float start;
			int loop;
		} object_particle_animation;
		struct
		{
			int id;
			char* shader;
			float params[4];
		} object_set_effect;
		struct
		{
			int id;
			int model;
		} object_set_model;
		struct
		{
			int id;
			LIMdlPose* pose;
		} object_set_pose;
		struct
		{
			int id;
			int value;
		} object_set_realized;
		struct
		{
			int id;
			LIMatTransform value;
		} object_set_transform;

		/* Overlay */
		/* TODO */

		/* Shader */
		/* TODO */
	};
};

LIAPICALL (LIRenMessage*, liren_render_message_new, (
	int type,
	int wait_for_result));

LIAPICALL (void, liren_render_message_free, (
	LIRenMessage* self));

#endif
