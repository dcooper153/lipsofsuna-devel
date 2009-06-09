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
 * \addtogroup liextcliOptions Options
 * @{
 */

#include <client/lips-client.h>
#include "ext-options.h"

static const void*
private_base ();

static int
private_init (liextOptions* self,
              liwdgManager* manager);

static void
private_free (liextOptions* self);

static int
private_event (liextOptions* self,
               liwdgEvent*   event);

static void
private_fsaa (liextOptions* self);

static void
private_global_shadows (liextOptions* self);

static void
private_local_shadows (liextOptions* self);

static void
private_shaders (liextOptions* self);

/****************************************************************************/

const liwdgClass liextOptionsType =
{
	LIWDG_BASE_DYNAMIC, private_base, "Options", sizeof (liextOptions),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_options_new (licliModule* module)
{
	liwdgWidget* self;

	self = liwdg_widget_new (module->widgets, &liextOptionsType);
	if (self == NULL)
		return NULL;
	LIEXT_WIDGET_OPTIONS (self)->module = module;

	return self;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgGroupType;
}

static int
private_init (liextOptions* self,
              liwdgManager* manager)
{
	int i;
	liwdgWidget* widgets[] =
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
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
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
	liwdg_widget_insert_callback (self->check_fsaa, LIWDG_CALLBACK_PRESSED, 0, private_fsaa, self);
	liwdg_check_set_text (LIWDG_CHECK (self->check_shaders), "Per-pixel lighting.");
	liwdg_widget_insert_callback (self->check_shaders, LIWDG_CALLBACK_PRESSED, 0, private_shaders, self);
	liwdg_check_set_text (LIWDG_CHECK (self->check_global_shadows), "Shadows for sunlight.");
	liwdg_widget_insert_callback (self->check_global_shadows, LIWDG_CALLBACK_PRESSED, 0, private_global_shadows, self);
	liwdg_check_set_text (LIWDG_CHECK (self->check_local_shadows), "Shadows for lamps.");
	liwdg_widget_insert_callback (self->check_local_shadows, LIWDG_CALLBACK_PRESSED, 0, private_local_shadows, self);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 0, 0, self->check_fsaa);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 0, 1, self->check_shaders);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 0, 2, self->check_local_shadows);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 0, 3, self->check_global_shadows);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->group);

	return 1;

error:
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			liwdg_widget_free (widgets[i]);
	}
	return 0;
}

static void
private_free (liextOptions* self)
{
}

static int
private_event (liextOptions* self,
               liwdgEvent*   event)
{
	return liwdgGroupType.event (LIWDG_WIDGET (self), event);
}

static void
private_fsaa (liextOptions* self)
{
	licli_window_set_fsaa (self->module->window, livid_features_get_max_samples () *
		liwdg_check_get_active (LIWDG_CHECK (self->check_fsaa)));
}

static void
private_global_shadows (liextOptions* self)
{
	lirnd_render_set_global_shadows (self->module->engine->render,
		liwdg_check_get_active (LIWDG_CHECK (self->check_global_shadows)));
}

static void
private_local_shadows (liextOptions* self)
{
	lirnd_render_set_local_shadows (self->module->engine->render,
		liwdg_check_get_active (LIWDG_CHECK (self->check_local_shadows)));
}

static void
private_shaders (liextOptions* self)
{
	lirnd_render_set_shaders_enabled (self->module->engine->render,
		liwdg_check_get_active (LIWDG_CHECK (self->check_shaders)));
	liwdg_check_set_active (LIWDG_CHECK (self->check_shaders),
		lirnd_render_get_shaders_enabled (self->module->engine->render));
}

/** @} */
/** @} */
/** @} */
