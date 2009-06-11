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
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#include <system/lips-system.h>
#include "ext-dialog.h"

static const void*
private_base ();

static int
private_init (liextDialog*    self,
              liwdgManager* manager);

static void
private_free (liextDialog* self);

static int
private_event (liextDialog* self,
               liwdgEvent*  event);

static int
private_generate (liextDialog* self);

static int
private_insert (liextDialog* self);

static int
private_remove (liextDialog* self);

/****************************************************************************/

const liwdgClass liextDialogType =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorDialog", sizeof (liextDialog),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_dialog_new (liwdgManager*   manager,
                  liextGenerator* generator)
{
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liextDialogType);
	if (self == NULL)
		return NULL;
	LIEXT_DIALOG (self)->generator = generator;
	LIEXT_DIALOG (self)->module = generator->module;

	return self;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgWindowType;
}

static int
private_init (liextDialog*   self,
              liwdgManager* manager)
{
	int i;
	liwdgWidget* widgets[] =
	{
		liwdg_label_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 1, 4))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	self->label_rule = widgets[(i = 0)];
	self->button_insert = widgets[++i];
	self->button_remove = widgets[++i];
	self->button_generate = widgets[++i];

	/* Configure widgets. */
	liwdg_label_set_text (LIWDG_LABEL (self->label_rule), "FIXME: Rule");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_insert), "Insert");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_remove), "Remove");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_generate), "Generate");
	liwdg_widget_insert_callback (self->button_insert, LIWDG_CALLBACK_PRESSED, 0, private_insert, self, NULL);
	liwdg_widget_insert_callback (self->button_remove, LIWDG_CALLBACK_PRESSED, 0, private_remove, self, NULL);
	liwdg_widget_insert_callback (self->button_generate, LIWDG_CALLBACK_PRESSED, 0, private_generate, self, NULL);

	/* Pack self. */
	liwdg_window_set_title (LIWDG_WINDOW (self), "Generator");
	liwdg_group_set_margins (LIWDG_GROUP (self), 5, 5, 5, 5);
	liwdg_group_set_spacings (LIWDG_GROUP (self), 5, 5);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 3, self->label_rule);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 2, self->button_insert);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 1, self->button_remove);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->button_generate);

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
private_free (liextDialog* self)
{
}

static int
private_generate (liextDialog* self)
{
	liext_generator_run (self->generator);
	return 0;
}

static int
private_insert (liextDialog* self)
{
	int rule;

	rule = liext_generator_find_rule (self->generator);
	if (rule == -1)
		liext_generator_insert_rule (self->generator);
	return 0;
}

static int
private_remove (liextDialog* self)
{
	int rule;

	rule = liext_generator_find_rule (self->generator);
	if (rule != -1)
		liext_generator_remove_rule (self->generator, rule);
	return 0;
}

static int
private_event (liextDialog* self,
               liwdgEvent*  event)
{
	char text[256];
	liextStatistics stats;

	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
#warning Might be too expensive to be run automatically at all.
		self->timer -= event->update.secs;
		if (self->timer <= 0.0f)
		{
			self->timer = 10.0f;
			liext_generator_get_statistics (self->generator, &stats);
			if (stats.selected_rule != -1)
			{
				snprintf (text, 256, "Known: %d\nExpand: %d\nSelect: %d\n",
					stats.rules_known, stats.rules_expand, stats.selected_rule);
			}
			else
			{
				snprintf (text, 256, "Known: %d\nExpand: %d\nSelect: None\n",
					stats.rules_known, stats.rules_expand);
			}
			liwdg_label_set_text (LIWDG_LABEL (self->label_rule), text);
		}
	}

	return liwdgWindowType.event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */
/** @} */
