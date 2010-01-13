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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup LIWdgImage Image
 * @{
 */

#include "widget-image.h"
#include "widget-manager.h"

static int
private_init (LIWdgImage*  self,
              LIWdgManager* manager);

static void
private_free (LIWdgImage* self);

static int
private_event (LIWdgImage* self,
               LIWdgEvent*  event);

static void
private_rebuild (LIWdgImage* self);

const LIWdgClass liwdg_widget_image =
{
	LIWDG_BASE_STATIC, &liwdg_widget_widget, "Image", sizeof (LIWdgImage),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event
};

/****************************************************************************/

LIWdgWidget*
liwdg_image_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdg_widget_image);
}

const char*
liwdg_image_get_image (LIWdgImage* self)
{
	return self->image;
}

int
liwdg_image_set_image (LIWdgImage* self,
                       const char* value)
{
	char* tmp;

	tmp = listr_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->image);
	self->image = tmp;
	liwdg_widget_set_style (LIWDG_WIDGET (self), value);
	private_rebuild (self);

	return 1;
}

/****************************************************************************/

static int
private_init (LIWdgImage*   self,
              LIWdgManager* manager)
{
	self->image = lisys_calloc (1, 1);
	if (self->image == NULL)
		return 0;
	private_rebuild (self);

	return 1;
}

static void
private_free (LIWdgImage* self)
{
	lisys_free (self->image);
}

static int
private_event (LIWdgImage* self,
               LIWdgEvent* event)
{
	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_KEY_PRESS:
			if (event->key.keycode != SDLK_RETURN)
				return 1;
			return lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self);
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			return lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self);
		case LIWDG_EVENT_TYPE_RENDER:
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			return 1;
	}

	return liwdg_widget_widget.event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (LIWdgImage* self)
{
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self), 1, 1);
}

/** @} */
/** @} */
