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
 * \addtogroup liextcliOptions Options
 * @{
 */

#include "ext-module.h"
#include "ext-options.h"

static int
private_init (LIExtOptions* self,
              LIWdgManager* manager);

static void
private_free (LIExtOptions* self);

static int
private_event (LIExtOptions* self,
               LIWdgEvent*   event);

static void
private_fsaa (LIExtOptions* self);

/****************************************************************************/

const LIWdgClass*
liext_widget_options ()
{
	static LIWdgClass clss =
	{
		NULL, "Options", sizeof (LIExtOptions),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	clss.base = liwdg_widget_group;
	return &clss;
}

LIWdgWidget*
liext_options_new (LICliClient* client)
{
	LIWdgWidget* self;

	self = liwdg_widget_new (client->widgets, liext_widget_options ());
	if (self == NULL)
		return NULL;
	LIEXT_WIDGET_OPTIONS (self)->client = client;

	return self;
}

/****************************************************************************/

static int
private_init (LIExtOptions* self,
              LIWdgManager* manager)
{
	int i;
	LIWdgWidget* widgets[] =
	{
		liwdg_group_new_with_size (manager, 1, 4),
		liwdg_check_new (manager),
		liwdg_check_new (manager),
		liwdg_check_new (manager),
		liwdg_check_new (manager),
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 1, 1))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (LIWdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	self->group = widgets[(i = 0)];
	self->check_fsaa = widgets[++i];
	self->check_global_shadows = widgets[++i];
	self->check_local_shadows = widgets[++i];
	self->check_shaders = widgets[++i];

	/* Pack admin group. */
	liwdg_check_set_text (LIWDG_CHECK (self->check_fsaa), "Anti-aliasing.");
	liwdg_check_set_text (LIWDG_CHECK (self->check_shaders), "Per-pixel lighting.");
	liwdg_check_set_text (LIWDG_CHECK (self->check_global_shadows), "Shadows for sunlight.");
	liwdg_check_set_text (LIWDG_CHECK (self->check_local_shadows), "Shadows for lamps.");
	liwdg_widget_insert_callback (self->check_fsaa, "pressed", private_fsaa, self);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 0, 0, self->check_fsaa);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 0, 1, self->check_shaders);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 0, 2, self->check_local_shadows);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 0, 3, self->check_global_shadows);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->group);

	return 1;

error:
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (LIWdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			liwdg_widget_free (widgets[i]);
	}
	return 0;
}

static void
private_free (LIExtOptions* self)
{
}

static int
private_event (LIExtOptions* self,
               LIWdgEvent*   event)
{
	return liwdg_widget_group ()->event (LIWDG_WIDGET (self), event);
}

static void
private_fsaa (LIExtOptions* self)
{
	licli_window_set_fsaa (self->client->window, livid_features_get_max_samples () *
		liwdg_check_get_active (LIWDG_CHECK (self->check_fsaa)));
}

/** @} */
/** @} */
/** @} */
