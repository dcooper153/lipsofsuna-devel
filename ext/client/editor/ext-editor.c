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
 * \addtogroup liextcliEditor Editor
 * @{
 */

#include <math.h>
#include <client/lips-client.h>
#include <widget/lips-widget.h>
#include "ext-editor.h"

static int
private_event (liextEditor* self,
               SDL_Event*   event);

static int
private_key_press (liextEditor*       self,
                   SDL_KeyboardEvent* event);

static int
private_mouse_press (liextEditor*          self,
                     SDL_MouseButtonEvent* event);

static int
private_mouse_motion (liextEditor*          self,
                      SDL_MouseMotionEvent* event);

/*****************************************************************************/

liextEditor*
liext_editor_new (licliModule* module)
{
	liextEditor* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liextEditor));
	if (self == NULL)
		return NULL;
	self->module = module;

	/* Allocate callbacks. */
	if (!lieng_engine_insert_call (module->engine, LICLI_CALLBACK_EVENT, -15,
	     	private_event, self, self->calls + 0))
	{
		liext_editor_free (self);
		return NULL;
	}

	return self;
}

void
liext_editor_free (liextEditor* self)
{
	lieng_engine_remove_calls (self->module->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));
	free (self);
}

void
liext_editor_begin_rotate (liextEditor* self,
                           int          x,
                           int          y)
{
	if (self->drag.mode != LIEXT_DRAG_NONE)
	{
		self->drag.mode = LIEXT_DRAG_NONE;
		liext_editor_transform_apply (self);
	}
	if (self->drag.mode != LIEXT_DRAG_ROTATE)
	{
		self->drag.mode = LIEXT_DRAG_ROTATE;
		self->drag.start = limat_vector_init (x, y, 0.0f);
	}
}

void
liext_editor_begin_translate (liextEditor* self,
                              int          x,
                              int          y)
{
	if (self->drag.mode != LIEXT_DRAG_NONE)
	{
		self->drag.mode = LIEXT_DRAG_NONE;
		liext_editor_transform_apply (self);
	}
	if (self->drag.mode != LIEXT_DRAG_TRANSLATE)
	{
		self->drag.mode = LIEXT_DRAG_TRANSLATE;
		self->drag.start = limat_vector_init (x, y, 0.0f);
	}
}

int
liext_editor_create (liextEditor*          self,
                     uint32_t              model,
                     const limatTransform* transform)
{
	liarcWriter* writer;

	writer = liarc_writer_new_packet (LINET_EXT_CLIENT_PACKET_EDITOR);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint8 (writer, LINET_EXT_EDITOR_PACKET_CREATE);
	liarc_writer_append_uint32 (writer, model);
	liarc_writer_append_float (writer, transform->position.x);
	liarc_writer_append_float (writer, transform->position.y);
	liarc_writer_append_float (writer, transform->position.z);
	liarc_writer_append_float (writer, transform->rotation.x);
	liarc_writer_append_float (writer, transform->rotation.y);
	liarc_writer_append_float (writer, transform->rotation.z);
	liarc_writer_append_float (writer, transform->rotation.w);
	licli_module_send (self->module, writer, GRAPPLE_RELIABLE);
	liarc_writer_free (writer);

	return 1;
}

/**
 * \brief Requests the server to remove all selected objects.
 *
 * \param self Editor.
 * \return Nonzero on success.
 */
int
liext_editor_destroy (liextEditor* self)
{
	liarcWriter* writer;
	lialgPtrdicIter iter;
	liengSelection* selection;

	LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
	{
		selection = iter.value;
		writer = liarc_writer_new_packet (LINET_EXT_CLIENT_PACKET_EDITOR);
		if (writer == NULL)
			return 0;
		liarc_writer_append_uint8 (writer, LINET_EXT_EDITOR_PACKET_DESTROY);
		liarc_writer_append_uint32 (writer, selection->object->id);
		licli_module_send (self->module, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

/**
 * \brief Requests the server to duplicate all selected objects.
 *
 * Due to technical issues, the old objects are left selected while the newly
 * created objects are not made selected. This is not a concern typically since
 * the new and old objects are usually identical in use, but it might cause
 * confusion when objects controlled by scripts are duplicated.
 *
 * \param self Editor.
 * \return Nonzero on success.
 */
int
liext_editor_duplicate (liextEditor* self)
{
	int code;
	lialgPtrdicIter iter;
	liengSelection* selection;
	limatTransform transform;

	LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
	{
		selection = iter.value;
		code = lieng_object_get_model_code (selection->object);
		licli_object_get_transform_target (selection->object, &transform);
		if (!liext_editor_create (self, code, &transform))
			return 0;
	}

	return 1;
}

void
liext_editor_rotate (liextEditor*           self,
                     const limatQuaternion* rotation)
{
	lialgPtrdicIter iter;
	liengSelection* selection;
	limatQuaternion newrotat;
	limatTransform transform;
	limatVector center;
	limatVector position;

	/* Get rotation center. */
	liext_editor_get_center (self, &center);

	/* Rotate each element. */
	LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
	{
		selection = iter.value;
		position = limat_vector_subtract (selection->transform.position, center);
		position = limat_quaternion_transform (*rotation, position);
		position = limat_vector_add (position, center);
		newrotat = limat_quaternion_multiply (*rotation, selection->transform.rotation);
		transform = limat_transform_init (position, newrotat);
		lieng_object_set_transform (selection->object, &transform);
	}
}

/**
 * \brief Sends a map save request to the server.
 *
 * \param self Editor.
 */
void
liext_editor_save (liextEditor* self)
{
	liarcWriter* writer;

	writer = liarc_writer_new_packet (LINET_EXT_CLIENT_PACKET_EDITOR);
	if (writer == NULL)
		return;
	liarc_writer_append_uint8 (writer, LINET_EXT_EDITOR_PACKET_SAVE);
	licli_module_send (self->module, writer, GRAPPLE_RELIABLE);
	liarc_writer_free (writer);
}

/**
 * \brief Sends a snap selection request to the server.
 *
 * \param self Editor.
 * \param grid Grid size.
 * \param rad Angle quantization in radians.
 */
void
liext_editor_snap (liextEditor* self,
                   float        grid,
                   float        rad)
{
	float a;
	float b;
	float c;
	lialgPtrdicIter iter;
	liengSelection* selection;
	limatTransform transform;
	limatVector position;

	/* Snap each element. */
	LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
	{
		selection = iter.value;
		position = selection->transform.position;
		position.x = limat_quantize (position.x, grid);
		position.y = limat_quantize (position.y, grid);
		position.z = limat_quantize (position.z, grid);

		/* FIXME: This is pretty coarse, but seems to work somehow. */
		limat_quaternion_get_euler (selection->transform.rotation, &a, &b, &c);
		a = limat_quantize (a, rad);
		b = limat_quantize (b, rad);
		c = limat_quantize (c, rad);
		selection->transform.rotation = limat_quaternion_euler (a, b, c);

		transform = limat_transform_init (position, selection->transform.rotation);
		lieng_object_set_transform (selection->object, &transform);
	}

	/* Send commands to server. */
	liext_editor_transform_apply (self);
}

/**
 * \brief Sets the translation delta of the selection.
 *
 * Translates each selected element from their original positions
 * by the specified amount.
 *
 * \param self Editor.
 * \param translation Translation vector relative to the starting point.
 */
void
liext_editor_translate (liextEditor*       self,
                        const limatVector* translation)
{
	lialgPtrdicIter iter;
	liengSelection* selection;
	limatTransform transform;
	limatVector position;

	LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
	{
		selection = iter.value;
		licli_object_get_transform_target (selection->object, &transform);
		position = limat_vector_add (selection->transform.position, *translation);
		transform = limat_transform_init (position, transform.rotation);
		lieng_object_set_transform (selection->object, &transform);
	}
}


/**
 * \brief Applies the transformation of the selection.
 *
 * \param self Editor.
 */
void
liext_editor_transform_apply (liextEditor* self)
{
	lialgPtrdicIter iter;
	liarcWriter* writer;
	liengSelection* selection;
	limatTransform transform;

	LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
	{
		/* Get target. */
		selection = iter.value;
		licli_object_get_transform_target (selection->object, &transform);
		selection->transform = transform;

		/* Send a transform packet. */
		writer = liarc_writer_new_packet (LINET_EXT_CLIENT_PACKET_EDITOR);
		if (writer == NULL)
			continue;
		liarc_writer_append_uint8 (writer, LINET_EXT_EDITOR_PACKET_TRANSFORM);
		liarc_writer_append_uint32 (writer, selection->object->id);
		liarc_writer_append_float (writer, transform.position.x);
		liarc_writer_append_float (writer, transform.position.y);
		liarc_writer_append_float (writer, transform.position.z);
		liarc_writer_append_float (writer, transform.rotation.x);
		liarc_writer_append_float (writer, transform.rotation.y);
		liarc_writer_append_float (writer, transform.rotation.z);
		liarc_writer_append_float (writer, transform.rotation.w);
		licli_module_send (self->module, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}
}

/**
 * \brief Restores the original transformation of the selection.
 *
 * \param self Editor.
 */
void
liext_editor_transform_cancel (liextEditor* self)
{
	lialgPtrdicIter iter;
	liengSelection* selection;
	limatTransform transform;

	LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
	{
		selection = iter.value;
		transform = selection->transform;
		lieng_object_set_transform (selection->object, &transform);
	}
}

void
liext_editor_get_center (liextEditor* self,
                         limatVector* value)
{
	lialgPtrdicIter iter;
	liengSelection* selection;

	*value = limat_vector_init (0.0f, 0.0f, 0.0f);
	if (self->module->engine->selection->size)
	{
		LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
		{
			selection = iter.value;
			*value = limat_vector_add (*value, selection->transform.position);
		}
		*value = limat_vector_multiply (*value, 1.0f / self->module->engine->selection->size);
	}
}

/*****************************************************************************/

static int
private_event (liextEditor* self,
	           SDL_Event*   event)
{
	switch (event->type)
	{
		case SDL_KEYDOWN:
			return private_key_press (self, &event->key);
		case SDL_MOUSEBUTTONDOWN:
			return private_mouse_press (self, &event->button);
		case SDL_MOUSEMOTION:
			return private_mouse_motion (self, &event->motion);
		default:
			return 1;
	}
}

static int
private_key_press (liextEditor*       self,
                   SDL_KeyboardEvent* event)
{
	int x;
	int y;

	if (self->drag.mode == LIEXT_DRAG_NONE)
		return 1;
	switch (event->keysym.sym)
	{
		case 'x':
			SDL_GetMouseState (&x, &y);
			if (self->drag.mode >= LIEXT_DRAG_ROTATE && self->drag.mode <= LIEXT_DRAG_ROTATEZ)
				self->drag.mode = LIEXT_DRAG_ROTATEX;
			else
				self->drag.mode = LIEXT_DRAG_TRANSLATEX;
			self->drag.start = limat_vector_init (x, y, 0.0f);
			break;
		case 'y':
			SDL_GetMouseState (&x, &y);
			if (self->drag.mode >= LIEXT_DRAG_ROTATE && self->drag.mode <= LIEXT_DRAG_ROTATEZ)
				self->drag.mode = LIEXT_DRAG_ROTATEY;
			else
				self->drag.mode = LIEXT_DRAG_TRANSLATEY;
			self->drag.start = limat_vector_init (x, y, 0.0f);
			break;
		case 'z':
			SDL_GetMouseState (&x, &y);
			if (self->drag.mode >= LIEXT_DRAG_ROTATE && self->drag.mode <= LIEXT_DRAG_ROTATEZ)
				self->drag.mode = LIEXT_DRAG_ROTATEZ;
			else
				self->drag.mode = LIEXT_DRAG_TRANSLATEZ;
			self->drag.start = limat_vector_init (x, y, 0.0f);
			break;
		default:
			return 1;
	}

	return 0;
}

static int
private_mouse_press (liextEditor*          self,
                     SDL_MouseButtonEvent* event)
{
	switch (event->button)
	{
		case 1:
			if (self->drag.mode != LIEXT_DRAG_NONE)
			{
				self->drag.mode = LIEXT_DRAG_NONE;
				liext_editor_transform_apply (self);
				return 0;
			}
			break;
		case 3:
			if (self->drag.mode != LIEXT_DRAG_NONE)
			{
				self->drag.mode = LIEXT_DRAG_NONE;
				liext_editor_transform_cancel (self);
				return 0;
			}
			break;
	}

	return 1;
}

static int
private_mouse_motion (liextEditor*          self,
                      SDL_MouseMotionEvent* event)
{
	int h;
	float amount;
	limatVector vx;
	limatVector vy;
	limatVector axis;
	limatVector center;
	limatVector delta;
	limatVector world;
	limatQuaternion quat;
	limatTransform transform;

	if (self->drag.mode == LIEXT_DRAG_ROTATE)
	{
		liext_editor_get_center (self, &world);
		lieng_camera_get_transform (self->module->camera, &transform);
		axis = limat_quaternion_transform (transform.rotation, limat_vector_init (0.0f, 0.0f, 1.0f));
		if (lieng_camera_project (self->module->camera, &world, &center))
		{
			licli_window_get_size (self->module->window, NULL, &h);
			center.y = h - center.y;
			amount = atan2 (self->drag.start.y - center.y, self->drag.start.x - center.x);
			amount -= atan2 (event->y - center.y, event->x - center.x);
			quat = limat_quaternion_rotation (amount, axis);
			liext_editor_rotate (self, &quat);
		}
	}
	else if (self->drag.mode == LIEXT_DRAG_ROTATEX)
	{
		amount = 0.02f * (event->x - self->drag.start.x);
		quat = limat_quaternion_rotation (amount, limat_vector_init (1.0f, 0.0f, 0.0f));
		liext_editor_rotate (self, &quat);
	}
	else if (self->drag.mode == LIEXT_DRAG_ROTATEY)
	{
		amount = 0.02f * (event->x - self->drag.start.x);
		quat = limat_quaternion_rotation (amount, limat_vector_init (0.0f, 1.0f, 0.0f));
		liext_editor_rotate (self, &quat);
	}
	else if (self->drag.mode == LIEXT_DRAG_ROTATEZ)
	{
		amount = 0.02f * (event->x - self->drag.start.x);
		quat = limat_quaternion_rotation (amount, limat_vector_init (0.0f, 0.0f, 1.0f));
		liext_editor_rotate (self, &quat);
	}
	else if (self->drag.mode == LIEXT_DRAG_TRANSLATE)
	{
		lieng_camera_get_transform (self->module->camera, &transform);
		vx = limat_quaternion_transform (transform.rotation, limat_vector_init (1.0f, 0.0f, 0.0f));
		vy = limat_quaternion_transform (transform.rotation, limat_vector_init (0.0f, -1.0f, 0.0f));
		vx = limat_vector_multiply (vx, 0.05f * (event->x - self->drag.start.x));
		vy = limat_vector_multiply (vy, 0.05f * (event->y - self->drag.start.y));
		delta = limat_vector_add (vx, vy);
		liext_editor_translate (self, &delta);
	}
	else if (self->drag.mode == LIEXT_DRAG_TRANSLATEX)
	{
		amount = 0.05f * (event->x - self->drag.start.x);
		delta = limat_vector_init (amount, 0.0f, 0.0f);
		liext_editor_translate (self, &delta);
	}
	else if (self->drag.mode == LIEXT_DRAG_TRANSLATEY)
	{
		amount = 0.05f * (event->x - self->drag.start.x);
		delta = limat_vector_init (0.0f, amount, 0.0f);
		liext_editor_translate (self, &delta);
	}
	else if (self->drag.mode == LIEXT_DRAG_TRANSLATEZ)
	{
		amount = 0.05f * (event->x - self->drag.start.x);
		delta = limat_vector_init (0.0f, 0.0f, amount);
		liext_editor_translate (self, &delta);
	}
	else
		return 1;
	return 0;
}

/** @} */
/** @} */
/** @} */
