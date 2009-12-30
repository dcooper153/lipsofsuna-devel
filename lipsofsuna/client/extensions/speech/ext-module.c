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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSpeech Speech
 * @{
 */

#include "ext-module.h"
#include "ext-speech.h"

static int
private_render_2d (liextModule* self,
                   liwdgRender* widget);

static int
private_tick (liextModule* self,
              float        secs);

/*****************************************************************************/

lisrvExtensionInfo liextInfo =
{
	LISRV_EXTENSION_VERSION, "Speech",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliModule* module)
{
	liextModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;

	/* Allocate objects. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (module->callbacks, module->engine, "render-2d", 1, private_render_2d, self, self->calls + 0) ||
	    !lical_callbacks_insert (module->callbacks, module->engine, "tick", 1, private_tick, self, self->calls + 1))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (module->script, "Speech", liextSpeechScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	lialgU32dicIter iter;

	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (licalHandle));

	/* Free objects. */
	if (self->objects != NULL)
	{
		LI_FOREACH_U32DIC (iter, self->objects)
			liext_object_free (iter.value);
		lialg_u32dic_free (self->objects);
	}

	lisys_free (self);
}

/**
 * \brief Sets the latest chat message for an object.
 *
 * \param self Module.
 * \param object Object.
 * \param message String.
 */
int
liext_module_set_speech (liextModule* self,
                         uint32_t     object,
                         const char*  message)
{
	int create;
	liengObject* engobj;
	liextObject* extobj;
	liextSpeech* speech;

	/* Find engine object. */
	create = 0;
	engobj = lieng_engine_find_object (self->module->engine, object);
	if (engobj == NULL)
		return 0;

	/* Find or create sound object. */
	extobj = lialg_u32dic_find (self->objects, object);
	if (extobj == NULL)
	{
		create = 1;
		extobj = liext_object_new ();
		if (extobj == NULL)
			return 0;
		if (!lialg_u32dic_insert (self->objects, object, extobj))
		{
			liext_object_free (extobj);
			return 0;
		}
	}

	/* Allocate new speech. */
	speech = liext_speech_new (self->module, message);
	if (speech == NULL)
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_object_free (extobj);
		}
		return 0;
	}
	if (!lialg_list_prepend (&extobj->speech, speech))
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_object_free (extobj);
		}
		liext_speech_free (speech);
		return 0;
	}

	return 1;
}

/*****************************************************************************/

liextObject*
liext_object_new ()
{
	return lisys_calloc (1, sizeof (liextObject));
}

void
liext_object_free (liextObject* self)
{
	lialgList* ptr;

	for (ptr = self->speech ; ptr != NULL ; ptr = ptr->next)
		liext_speech_free (ptr->data);
	lialg_list_free (self->speech);
	lisys_free (self);
}

/*****************************************************************************/

static int
private_render_2d (liextModule* self,
                   liwdgRender* widget)
{
	int width;
	lialgU32dicIter iter;
	lialgList* ptr;
	liextObject* object;
	liextSpeech* speech;
	limatVector win;

	glEnable (GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glDisable (GL_CULL_FACE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		object = iter.value;

		/* Project start offset. */
		if (!lialg_camera_project (self->module->camera, &object->position, &win))
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
			glPushMatrix ();
			glTranslatef (win.x - width, win.y, 0.0f);
			glScalef (1.0f, -1.0f, 1.0f);
			glColor4f (0.0f, 0.0f, 0.0f, speech->alpha);
			lifnt_layout_render (speech->text, 1, -1);
			glColor4f (1.0f, 1.0f, 1.0f, speech->alpha);
			lifnt_layout_render (speech->text, 0, 0);
			glPopMatrix ();
		}
	}

	return 1;
}

static int
private_tick (liextModule* self,
              float        secs)
{
	float t;
	lialgU32dicIter iter;
	lialgList* ptr;
	lialgList* next;
	liengObject* engobj;
	liextObject* extobj;
	liextSpeech* speech;
	limatAabb bounds;
	limatTransform transform;

	/* Update speech. */
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		extobj = iter.value;
		engobj = lieng_engine_find_object (self->module->engine, iter.key);
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
			liext_object_free (extobj);
		}
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
