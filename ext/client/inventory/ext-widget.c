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
 * \addtogroup liextcliInventory Inventory
 * @{
 */

#include <class/lips-class.h>
#include <client/lips-client.h>
#include "ext-inventory.h"
#include "ext-widget.h"

#define LIEXT_SLOT_SIZE 40

static const void*
private_base ();

static int
private_init (liextInventoryWidget* self,
              liwdgManager*         manager);

static void
private_free (liextInventoryWidget* self);

static int
private_event (liextInventoryWidget* self,
               liwdgEvent*           event);

static void
private_calculate_modelview (liextInventoryWidget* self,
                             liengObject*          object,
                             liwdgRect*            frame,
                             float                 scale,
                             limatMatrix*          modelview);

static const liwdgWidgetClass
liextInventoryWidgetType =
{
	LI_CLASS_BASE_DYNAMIC, private_base, "InventoryWidget", sizeof (liextInventoryWidget),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

liwdgWidget*
liext_inventory_widget_new (liextInventory* inventory)
{

	const float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	const float ambient[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
	const limatVector dir = limat_vector_init (0.707f, 0.707f, 0.0f);
	liwdgWidget* self;

	/* Allocate self. */
	self = li_instance_new (&liextInventoryWidgetType, inventory->module->module->widgets);
	if (self == NULL)
		return NULL;
	LIEXT_INVENTORY_WIDGET (self)->module = inventory->module;
	LIEXT_INVENTORY_WIDGET (self)->inventory = inventory;

	/* Allocate light. */
	LIEXT_INVENTORY_WIDGET (self)->light = lirnd_light_new_directional (inventory->module->module->engine->render, color);
	if (LIEXT_INVENTORY_WIDGET (self)->light == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	lirnd_light_set_ambient (LIEXT_INVENTORY_WIDGET (self)->light, ambient);
	lirnd_light_set_direction (LIEXT_INVENTORY_WIDGET (self)->light, &dir);

	return self;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgWidgetType;
}

static int
private_init (liextInventoryWidget* self,
              liwdgManager*         manager)
{
	if (!liwdg_widget_register_callback (LIWDG_WIDGET (self), LIEXT_CALLBACK_ACTIVATE, lical_marshal_DATA_INT))
		return 0;
	return 1;
}

static void
private_free (liextInventoryWidget* self)
{
	if (self->light != NULL)
		lirnd_light_free (self->light);
}

static int
private_event (liextInventoryWidget* self,
               liwdgEvent*           event)
{
	int i;
	int w;
	int x;
	int y;
	int slot;
	int size;
	float scale;
	liengObject* object;
	limatAabb bounds;
	limatMatrix modelview;
	limatMatrix projection;
	limatVector center;
	lirndContext context;
	lirndRender* render;
	liwdgRect rect;
	liwdgRect frame;
	liwdgSubimage* subimg;

	if (event->type == LIWDG_EVENT_TYPE_BUTTON_PRESS)
	{
		if (!self->slotsize.width || self->inventory == NULL)
			return 1;
		liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
		x = event->button.x - rect.x;
		y = event->button.y - rect.y;
		size = liext_inventory_get_size (self->inventory);
		if (x >= self->slotsize.width * size ||
		    y >= self->slotsize.height)
			return 1;
		slot = x / self->slotsize.width;
		return lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIEXT_CALLBACK_ACTIVATE, slot);
	}
	if (event->type == LIWDG_EVENT_TYPE_RENDER)
	{
		liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
		size = liext_inventory_get_size (self->inventory);
		render = self->module->module->engine->render;

		/* Draw frame. */
		subimg = liwdg_manager_find_style (LIWDG_WIDGET (self)->manager, "inventory");
		if (subimg != NULL)
		{
			w = (int) ceil ((float) LIEXT_SLOT_SIZE / LI_MAX (0, subimg->w[1]));
			w = subimg->w[0] + w * subimg->w[1] + subimg->w[2];
			for (i = 0 ; i < size ; i++)
			{
				frame = rect;
				frame.x += i * w;
				frame.width = w;
				liwdg_widget_paint (LIWDG_WIDGET (self), "inventory", &frame);
			}
		}
		else
			w = LIEXT_SLOT_SIZE;

		/* Draw items. */
		lirnd_context_init (&context, render);
		for (i = 0 ; i < size ; i++)
		{
			/* Get slot item. */
			object = liext_inventory_get_object (self->inventory, i);
			if (object == NULL)
				continue;

			/* Calculate frame size. */
			frame = rect;
			frame.x += i * w;
			frame.width = w;
			if (subimg != NULL)
			{
				frame.x += subimg->pad[1];
				frame.y += subimg->pad[3];
				frame.width -= subimg->pad[1] + subimg->pad[2];
				frame.height -= subimg->pad[0] + subimg->pad[3];
			}

			/* Calculate scale factor. */
			lieng_object_get_bounds (object, &bounds);
			center = limat_vector_add (bounds.min, bounds.max);
			center = limat_vector_multiply (center, 0.5f);
#warning FIXME: Hardcoded inventory scale factor.
//			scale = 2.0f * LI_MAX (bounds.max.x - center.x, bounds.max.y - center.y);
//			scale = LI_MIN (frame.width, frame.height) / LI_MAX (1, scale);
			scale = 50.0f;

			/* Render the object. */
			glMatrixMode (GL_PROJECTION);
			glPushMatrix ();
			glMatrixMode (GL_MODELVIEW);
			glPushMatrix ();
			glClear (GL_DEPTH_BUFFER_BIT);
			private_calculate_modelview (self, object, &frame, scale, &modelview);
			lirnd_context_set_modelview (&context, &modelview);
			liwdg_manager_get_projection (LIWDG_WIDGET (self)->manager, &projection);
			lirnd_context_set_projection (&context, &projection);
			lirnd_context_set_lights (&context, &self->light, 1);
			lirnd_object_render (object->render, &context);
			glMatrixMode (GL_PROJECTION);
			glPopMatrix ();
			glMatrixMode (GL_MODELVIEW);
			glPopMatrix ();
		}
		lirnd_context_unbind (&context);

		return 1;
	}
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
		/* Calculate slot size. */
		subimg = liwdg_manager_find_style (LIWDG_WIDGET (self)->manager, "inventory");
		if (subimg != NULL)
		{
			self->slotsize.width = (int) ceil ((float) LIEXT_SLOT_SIZE / LI_MAX (0, subimg->w[1]));
			self->slotsize.width = subimg->w[0] + self->slotsize.width * subimg->w[1] + subimg->w[2];
			self->slotsize.height = LIEXT_SLOT_SIZE;
		}
		else
		{
			self->slotsize.width = LIEXT_SLOT_SIZE;
			self->slotsize.height = LIEXT_SLOT_SIZE;
		}
		/* Calculate total size. */
		size = liext_inventory_get_size (self->inventory);
		liwdg_widget_set_request (LIWDG_WIDGET (self), size * self->slotsize.width, self->slotsize.height);
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

static void
private_calculate_modelview (liextInventoryWidget* self,
                             liengObject*          object,
                             liwdgRect*            frame,
                             float                 scale,
                             limatMatrix*          modelview)
{
	limatMatrix m0;
	limatMatrix m1;
	limatMatrix m2;
	limatMatrix m3;
	limatTransform transform;
	limdlNode* node;

	m0 = limat_matrix_translation (frame->x + 0.5 * frame->width, frame->y + 0.5 * frame->height, 0.0f);
	m1 = limat_matrix_scale (scale, scale, scale);
	m2 = limat_matrix_rotation (0.55f, 0.0f, 1.0f, 0.0f);
	node = limdl_pose_find_node (object->render->pose.pose, "#root");
	if (node != NULL)
	{
		limdl_node_get_pose_transform (node, &transform);
		transform = limat_transform_invert (transform);
		m3 = limat_convert_transform_to_matrix (transform);
	}
	else
		m3 = limat_matrix_identity ();

	*modelview = limat_matrix_multiply (m0, m1);
	*modelview = limat_matrix_multiply (*modelview, m2);
	*modelview = limat_matrix_multiply (*modelview, m3);
}

/** @} */
/** @} */
/** @} */
