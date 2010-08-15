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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSpeech Speech
 * @{
 */

#include "ext-module.h"
#include "ext-speech.h"

static int private_tick (
	LIExtModule* self,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_speech_info =
{
	LIMAI_EXTENSION_VERSION, "Speech",
	liext_speeches_new,
	liext_speeches_free
};

LIExtModule* liext_speeches_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;

	/* Make sure the widgets extension is loaded. */
	if (!limai_program_insert_extension (program, "widgets"))
	{
		lisys_free (self);
		return NULL;
	}
	self->client = limai_program_find_component (program, "client");
	self->widgets = limai_program_find_component (program, "widgets");
	self->video = &self->client->video;

	/* Allocate objects. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "tick", 1, private_tick, self, self->calls + 0))
	{
		liext_speeches_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Speech", liext_script_speech, self);

	return self;
}

void liext_speeches_free (
	LIExtModule* self)
{
	LIAlgU32dicIter iter;

	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Free objects. */
	if (self->objects != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->objects)
			liext_speech_object_free (iter.value);
		lialg_u32dic_free (self->objects);
	}

	lisys_free (self);
}

void liext_speeches_render (
	LIExtModule* self)
{
	int width;
	GLint viewport[4];
	LIAlgU32dicIter iter;
	LIAlgList* ptr;
	LIExtObject* object;
	LIExtSpeech* speech;
	LIMatVector win;

	/* Set 2D mode. */
	glGetIntegerv (GL_VIEWPORT, viewport);
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity();
	glOrtho (0, viewport[2], 0, viewport[3], -100.0f, 100.0f);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();

	self->video->glEnable (GL_TEXTURE_2D);
	self->video->glEnable (GL_BLEND);
	self->video->glDisable (GL_CULL_FACE);
	self->video->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		object = iter.value;

		/* Project start offset. */
		if (!lialg_camera_project (self->client->camera, &object->position, &win))
			continue;
		if (win.z < 0.0f)
			continue;
		win.y -= 5;

		/* Render all messages. */
		for (ptr = object->speech ; ptr != NULL ; ptr = ptr->next)
		{
			speech = ptr->data;
			win.y += lifnt_layout_get_height (speech->text);
			width = lifnt_layout_get_width (speech->text) / 2;
			self->video->glPushMatrix ();
			self->video->glTranslatef (win.x - width, win.y, 0.0f);
			self->video->glScalef (1.0f, -1.0f, 1.0f);
			self->video->glColor4f (0.0f, 0.0f, 0.0f, speech->alpha);
			lifnt_layout_render (speech->text, 1, -1);
			self->video->glColor4f (1.0f, 1.0f, 1.0f, speech->alpha);
			lifnt_layout_render (speech->text, 0, 0);
			self->video->glPopMatrix ();
		}
	}

	 /* Reset mode. */
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}

/**
 * \brief Sets the latest chat message for an object.
 *
 * \param self Module.
 * \param object Object.
 * \param message String.
 */
int liext_speeches_set_speech (
	LIExtModule* self,
	uint32_t     object,
	const char*  message)
{
	int create;
	LIEngObject* engobj;
	LIExtObject* extobj;
	LIExtSpeech* speech;

	/* Find engine object. */
	create = 0;
	engobj = lieng_engine_find_object (self->client->engine, object);
	if (engobj == NULL)
		return 0;

	/* Find or create sound object. */
	extobj = lialg_u32dic_find (self->objects, object);
	if (extobj == NULL)
	{
		create = 1;
		extobj = liext_speech_object_new ();
		if (extobj == NULL)
			return 0;
		if (!lialg_u32dic_insert (self->objects, object, extobj))
		{
			liext_speech_object_free (extobj);
			return 0;
		}
	}

	/* Allocate new speech. */
	speech = liext_speech_new (self, message);
	if (speech == NULL)
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_speech_object_free (extobj);
		}
		return 0;
	}
	if (!lialg_list_prepend (&extobj->speech, speech))
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_speech_object_free (extobj);
		}
		liext_speech_free (speech);
		return 0;
	}

	return 1;
}

/*****************************************************************************/

LIExtObject* liext_speech_object_new ()
{
	return lisys_calloc (1, sizeof (LIExtObject));
}

void liext_speech_object_free (
	LIExtObject* self)
{
	LIAlgList* ptr;

	for (ptr = self->speech ; ptr != NULL ; ptr = ptr->next)
		liext_speech_free (ptr->data);
	lialg_list_free (self->speech);
	lisys_free (self);
}

/*****************************************************************************/

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	float t;
	LIAlgU32dicIter iter;
	LIAlgList* ptr;
	LIAlgList* next;
	LIEngObject* engobj;
	LIExtObject* extobj;
	LIExtSpeech* speech;
	LIMatAabb bounds;
	LIMatTransform transform;

	/* Update speech. */
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		extobj = iter.value;
		engobj = lieng_engine_find_object (self->client->engine, iter.key);
		if (engobj != NULL)
		{
			/* Update speech offset. */
			lieng_object_get_transform (engobj, &transform);
			lieng_object_get_bounds (engobj, &bounds);
			extobj->position = transform.position;
			extobj->position.y += bounds.max.y;

			/* Update speech timing. */
			for (ptr = extobj->speech ; ptr != NULL ; ptr = next)
			{
				next = ptr->next;
				speech = ptr->data;
				speech->timer += secs;
				if (speech->timer > LIEXT_SPEECH_TIMEOUT)
				{
					liext_speech_free (speech);
					lialg_list_remove (&extobj->speech, ptr);
				}
				else
				{
					t = speech->timer / LIEXT_SPEECH_TIMEOUT;
					if (t < 0.5)
						speech->alpha = 1.0f;
					else
						speech->alpha = 1.0f - powf (2.0f * t - 0.5, 4);
				}
			}
		}
		if (engobj == NULL || extobj->speech == NULL)
		{
			lialg_u32dic_remove (self->objects, iter.key);
			liext_speech_object_free (extobj);
		}
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
