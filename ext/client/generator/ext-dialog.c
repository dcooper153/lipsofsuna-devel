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

#define LIEXT_PREVIEW_CENTER 8160

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
private_add_brush (liextDialog* self,
                   liwdgWidget* widget);

static int
private_add_rule (liextDialog* self,
                  liwdgWidget* widget);

static int
private_add_stroke (liextDialog* self,
                    liwdgWidget* widget);

static int
private_edit_stroke (liextDialog* self,
                     liwdgWidget* widget);

static int
private_remove_brush (liextDialog* self,
                      liwdgWidget* widget);

static int
private_remove_rule (liextDialog* self,
                     liwdgWidget* widget);

static int
private_remove_stroke (liextDialog* self,
                       liwdgWidget* widget);

static int
private_brush_selected (liextDialog* self,
                        liwdgWidget* widget);

static int
private_rule_selected (liextDialog* self,
                       liwdgWidget* widget);

static int
private_stroke_selected (liextDialog* self,
                         liwdgWidget* widget);

static void
private_move (liextDialog* self,
              int          dx,
              int          dy,
              int          dz);

static int
private_move_up (liextDialog* self,
                 liwdgWidget* widget);

static int
private_move_down (liextDialog* self,
                   liwdgWidget* widget);

static int
private_move_left (liextDialog* self,
                   liwdgWidget* widget);

static int
private_move_right (liextDialog* self,
                    liwdgWidget* widget);

static void
private_render_preview (liwdgWidget* widget,
                        liextDialog* self);

static void
private_populate_brushes (liextDialog* self);

static void
private_populate_rules (liextDialog* self);

static void
private_populate_strokes (liextDialog* self);

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
	liextDialog* data;
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liextDialogType);
	if (self == NULL)
		return NULL;
	data = LIEXT_DIALOG (self);
	data->generator = generator;
	data->module = generator->module;

	/* Populate brush list. */
	private_populate_brushes (LIEXT_DIALOG (self));

	/* Initialize preview widget. */
	data->render_strokes = liwdg_render_new (manager, generator->scene);
	if (data->render_strokes == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	LIWDG_RENDER (data->render_strokes)->custom_render_func = (void*) private_render_preview;
	LIWDG_RENDER (data->render_strokes)->custom_render_data = self;
	liwdg_widget_set_request (data->render_strokes, 320, 240);
	liwdg_group_set_col_expand (LIWDG_GROUP (data->group_column), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (data->group_column), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (data->group_column), 0, 1, data->render_strokes);

	return self;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgWindowType;
}

static int
private_init (liextDialog*  self,
              liwdgManager* manager)
{
	int i;
	liwdgWidget* group_buttons;
	liwdgWidget* group_brushes;
	liwdgWidget* group_rules;
	liwdgWidget* widgets[] =
	{
		liwdg_group_new_with_size (manager, 7, 1),
		liwdg_group_new_with_size (manager, 1, 5),
		liwdg_group_new_with_size (manager, 1, 5),
		liwdg_group_new_with_size (manager, 1, 4),
		liwdg_group_new_with_size (manager, 1, 0),
		liwdg_group_new_with_size (manager, 1, 0),
		liwdg_group_new_with_size (manager, 1, 0),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 4, 1))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	group_buttons = widgets[(i = 0)];
	group_brushes = widgets[++i];
	group_rules = widgets[++i];
	self->group_column = widgets[++i];
	self->group_brushes = widgets[++i];
	self->group_rules = widgets[++i];
	self->group_strokes = widgets[++i];
	self->button_move_up = widgets[++i];
	self->button_move_down = widgets[++i];
	self->button_move_left = widgets[++i];
	self->button_move_right = widgets[++i];
	self->button_add_brush = widgets[++i];
	self->button_remove_brush = widgets[++i];
	self->button_add_rule = widgets[++i];
	self->button_remove_rule = widgets[++i];
	self->button_add_stroke = widgets[++i];
	self->button_edit_stroke = widgets[++i];
	self->button_remove_stroke = widgets[++i];

	/* Configure widgets. */
	liwdg_group_set_spacings (LIWDG_GROUP (self->group_brushes), 0, 0);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->group_brushes), 0, 1);
	liwdg_group_set_spacings (LIWDG_GROUP (self->group_rules), 0, 0);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->group_rules), 0, 1);
	liwdg_group_set_spacings (LIWDG_GROUP (self->group_strokes), 0, 0);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->group_strokes), 0, 1);
	liwdg_button_set_text (LIWDG_BUTTON (self->button_move_up), "↑");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_move_down), "↓");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_move_left), "←");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_move_right), "→");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_add_brush), "Add");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_remove_brush), "Remove");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_add_rule), "Add");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_remove_rule), "Remove");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_add_stroke), "Add");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_edit_stroke), "Edit");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_remove_stroke), "Remove");
	liwdg_widget_insert_callback (self->button_move_up, LIWDG_CALLBACK_PRESSED, 0, private_move_up, self, NULL);
	liwdg_widget_insert_callback (self->button_move_down, LIWDG_CALLBACK_PRESSED, 0, private_move_down, self, NULL);
	liwdg_widget_insert_callback (self->button_move_left, LIWDG_CALLBACK_PRESSED, 0, private_move_left, self, NULL);
	liwdg_widget_insert_callback (self->button_move_right, LIWDG_CALLBACK_PRESSED, 0, private_move_right, self, NULL);
	liwdg_widget_insert_callback (self->button_add_brush, LIWDG_CALLBACK_PRESSED, 0, private_add_brush, self, NULL);
	liwdg_widget_insert_callback (self->button_remove_brush, LIWDG_CALLBACK_PRESSED, 0, private_remove_brush, self, NULL);
	liwdg_widget_insert_callback (self->button_add_rule, LIWDG_CALLBACK_PRESSED, 0, private_add_rule, self, NULL);
	liwdg_widget_insert_callback (self->button_remove_rule, LIWDG_CALLBACK_PRESSED, 0, private_remove_rule, self, NULL);
	liwdg_widget_insert_callback (self->button_add_stroke, LIWDG_CALLBACK_PRESSED, 0, private_add_stroke, self, NULL);
	liwdg_widget_insert_callback (self->button_edit_stroke, LIWDG_CALLBACK_PRESSED, 0, private_edit_stroke, self, NULL);
	liwdg_widget_insert_callback (self->button_remove_stroke, LIWDG_CALLBACK_PRESSED, 0, private_remove_stroke, self, NULL);

	/* Brushes. */
	liwdg_group_set_row_expand (LIWDG_GROUP (group_brushes), 2, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_brushes), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_brushes), 0, 4, liwdg_label_new_with_text (manager, "Brush"));
	liwdg_group_set_child (LIWDG_GROUP (group_brushes), 0, 3, self->group_brushes);
	liwdg_group_set_child (LIWDG_GROUP (group_brushes), 0, 1, self->button_add_brush);
	liwdg_group_set_child (LIWDG_GROUP (group_brushes), 0, 0, self->button_remove_brush);

	/* Rules. */
	liwdg_group_set_row_expand (LIWDG_GROUP (group_rules), 2, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_rules), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_rules), 0, 4, liwdg_label_new_with_text (manager, "Rule"));
	liwdg_group_set_child (LIWDG_GROUP (group_rules), 0, 3, self->group_rules);
	liwdg_group_set_child (LIWDG_GROUP (group_rules), 0, 1, self->button_add_rule);
	liwdg_group_set_child (LIWDG_GROUP (group_rules), 0, 0, self->button_remove_rule);

	/* Strokes. */
	liwdg_group_set_col_expand (LIWDG_GROUP (group_buttons), 0, 1);
	liwdg_group_set_homogeneous (LIWDG_GROUP (group_buttons), 1);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 0, 0, self->button_move_left);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 1, 0, self->button_move_down);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 2, 0, self->button_move_up);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 3, 0, self->button_move_right);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 4, 0, self->button_add_stroke);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 5, 0, self->button_remove_stroke);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 6, 0, self->button_edit_stroke);
	liwdg_group_set_child (LIWDG_GROUP (self->group_column), 0, 2, self->group_strokes);
	liwdg_group_set_child (LIWDG_GROUP (self->group_column), 0, 0, group_buttons);

	/* Pack self. */
	liwdg_window_set_title (LIWDG_WINDOW (self), "Generator");
	liwdg_group_set_margins (LIWDG_GROUP (self), 5, 5, 5, 5);
	liwdg_group_set_spacings (LIWDG_GROUP (self), 5, 5);
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, group_brushes);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 0, group_rules);
	liwdg_group_set_child (LIWDG_GROUP (self), 2, 0, self->group_column);

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
private_event (liextDialog* self,
               liwdgEvent*  event)
{
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
		self->timer -= event->update.secs;
		if (self->timer <= 0.0f)
		{
			self->timer = 10.0f;
			/* FIXME: Do we need to do anything here anymore? */
		}
	}

	return liwdgWindowType.event (LIWDG_WIDGET (self), event);
}

static int
private_add_brush (liextDialog* self,
                   liwdgWidget* widget)
{
	ligenBrush* brush;

	/* Create new brush. */
	brush = ligen_brush_new (10, 10, 10);
	if (brush == NULL)
		return 0;
	if (!ligen_generator_insert_brush (self->generator->generator, brush))
	{
		ligen_brush_free (brush);
		return 0;
	}
	private_populate_brushes (self);
	private_populate_rules (self);
	private_populate_strokes (self);

	return 0;
}

static int
private_add_rule (liextDialog* self,
                  liwdgWidget* widget)
{
	ligenBrush* brush;
	ligenRule* rule;

	/* Get active brush. */
	if (self->generator->generator->brushes.count <= self->active_brush)
		return 0;
	brush = self->generator->generator->brushes.array[self->active_brush];

	/* Create new rule. */
	rule = ligen_rule_new ();
	if (rule == NULL)
		return 0;
	if (!ligen_brush_insert_rule (brush, rule))
	{
		ligen_rule_free (rule);
		return 0;
	}
	private_populate_rules (self);
	private_populate_strokes (self);

	return 0;
}

static int
private_add_stroke (liextDialog* self,
                    liwdgWidget* widget)
{
	ligenBrush* brush;
	ligenRule* rule;

	/* Get active rule. */
	if (self->generator->generator->brushes.count <= self->active_brush)
		return 0;
	brush = self->generator->generator->brushes.array[self->active_brush];
	if (brush->rules.count <= self->active_rule)
		return 0;
	rule = brush->rules.array[self->active_rule];

	/* Create new stroke. */
	ligen_rule_insert_stroke (rule, 0, 0, 0, 0, 0);
	private_populate_strokes (self);

	return 0;
}

static int
private_edit_stroke (liextDialog* self,
                     liwdgWidget* widget)
{
	ligenBrush* brush;
	ligenRule* rule;
	ligenRulestroke* stroke;

	/* Get active stroke. */
	if (self->generator->generator->brushes.count <= self->active_brush)
		return 0;
	brush = self->generator->generator->brushes.array[self->active_brush];
	if (brush->rules.count <= self->active_rule)
		return 0;
	rule = brush->rules.array[self->active_rule];
	if (rule->strokes.count <= self->active_stroke)
		return 0;
	stroke = rule->strokes.array + self->active_stroke;

	/* FIXME: Popup a proper dialog. */
	stroke->brush += 1;
	stroke->brush %= self->generator->generator->brushes.count;
	private_populate_strokes (self);

	return 0;
}

static int
private_remove_brush (liextDialog* self,
                      liwdgWidget* widget)
{
	private_populate_brushes (self);
	private_populate_rules (self);
	private_populate_strokes (self);

	return 0;
}

static int
private_remove_rule (liextDialog* self,
                     liwdgWidget* widget)
{
	private_populate_rules (self);
	private_populate_strokes (self);

	return 0;
}

static int
private_remove_stroke (liextDialog* self,
                       liwdgWidget* widget)
{
	private_populate_strokes (self);

	return 0;
}

static int
private_brush_selected (liextDialog* self,
                        liwdgWidget* widget)
{
	int i;
	int h;
	liwdgWidget* child;

	/* Remove highlight. */
	liwdg_group_get_size (LIWDG_GROUP (self->group_brushes), NULL, &h);
	i = h - self->active_brush - 1;
	if (i >= 0)
	{
		child = liwdg_group_get_child (LIWDG_GROUP (self->group_brushes), 0, i);
		liwdg_label_set_highlight (LIWDG_LABEL (child), 0);
	}

	/* Activate new brush. */
	for (i = 0 ; i < h ; i++)
	{
		child = liwdg_group_get_child (LIWDG_GROUP (self->group_brushes), 0, i);
		if (child == widget)
		{
			liwdg_label_set_highlight (LIWDG_LABEL (child), 1);
			self->active_brush = h - i - 1;
			self->active_rule = 0;
			self->active_stroke = 0;
			private_populate_rules (self);
			private_populate_strokes (self);
			break;
		}
	}

	return 0;
}

static int
private_rule_selected (liextDialog* self,
                       liwdgWidget* widget)
{
	int i;
	int h;
	liwdgWidget* child;

	/* Remove highlight. */
	liwdg_group_get_size (LIWDG_GROUP (self->group_rules), NULL, &h);
	i = h - self->active_rule - 1;
	if (i >= 0)
	{
		child = liwdg_group_get_child (LIWDG_GROUP (self->group_rules), 0, i);
		liwdg_label_set_highlight (LIWDG_LABEL (child), 0);
	}

	/* Activate new rule. */
	for (i = 0 ; i < h ; i++)
	{
		child = liwdg_group_get_child (LIWDG_GROUP (self->group_rules), 0, i);
		if (child == widget)
		{
			liwdg_label_set_highlight (LIWDG_LABEL (child), 1);
			self->active_rule = h - i - 1;
			self->active_stroke = 0;
			private_populate_strokes (self);
			break;
		}
	}

	return 0;
}

static int
private_stroke_selected (liextDialog* self,
                         liwdgWidget* widget)
{
	int i;
	int h;
	liwdgWidget* child;

	/* Remove highlight. */
	liwdg_group_get_size (LIWDG_GROUP (self->group_strokes), NULL, &h);
	i = h - self->active_stroke - 1;
	if (i >= 0)
	{
		child = liwdg_group_get_child (LIWDG_GROUP (self->group_strokes), 0, i);
		liwdg_label_set_highlight (LIWDG_LABEL (child), 0);
	}

	/* Activate new stroke. */
	for (i = 0 ; i < h ; i++)
	{
		child = liwdg_group_get_child (LIWDG_GROUP (self->group_strokes), 0, i);
		if (child == widget)
		{
			liwdg_label_set_highlight (LIWDG_LABEL (child), 1);
			self->active_stroke = h - i - 1;
			break;
		}
	}

	return 0;
}

static void
private_move (liextDialog* self,
              int          dx,
              int          dy,
              int          dz)
{
	ligenBrush* brush;
	ligenRule* rule;
	ligenRulestroke* stroke;

	/* Get active rule. */
	if (self->generator->generator->brushes.count <= self->active_brush)
		return;
	brush = self->generator->generator->brushes.array[self->active_brush];
	if (brush->rules.count <= self->active_rule)
		return;
	rule = brush->rules.array[self->active_rule];
	if (rule->strokes.count <= self->active_stroke)
		return;
	stroke = rule->strokes.array + self->active_stroke;

	/* Move rule. */
	stroke->pos[0] += dx;
	stroke->pos[1] += dy;
	stroke->pos[2] += dz;
	private_populate_strokes (self);

	return;
}

static int
private_move_up (liextDialog* self,
                 liwdgWidget* widget)
{
	private_move (self, 0, 0, 1);
	return 0;
}

static int
private_move_down (liextDialog* self,
                   liwdgWidget* widget)
{
	private_move (self, 0, 0, -1);
	return 0;
}

static int
private_move_left (liextDialog* self,
                   liwdgWidget* widget)
{
	private_move (self, 1, 0, 0);
	return 0;
}

static int
private_move_right (liextDialog* self,
                    liwdgWidget* widget)
{
	private_move (self, -1, 0, 0);
	return 0;
}


static void
private_render_preview (liwdgWidget* widget,
                        liextDialog* self)
{
	int i;
	int x[2];
	int y[2];
	int z[2];
	ligenStroke* stroke;

	/* Render stroke bounds. */
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_LIGHTING);
	glDisable (GL_TEXTURE_2D);
	glBegin (GL_LINES);
	for (i = 0 ; i < self->generator->generator->world.count ; i++)
	{
		stroke = self->generator->generator->world.array + i;
		if (stroke->brush == self->active_brush &&
		    stroke->pos[0] == LIEXT_PREVIEW_CENTER &&
		    stroke->pos[1] == LIEXT_PREVIEW_CENTER &&
		    stroke->pos[2] == LIEXT_PREVIEW_CENTER)
			glColor3f (1.0f, 0.0f, 0.0f);
		else
			glColor3f (0.0f, 1.0f, 0.0f);
		x[0] = LIVOX_TILE_WIDTH * (stroke->pos[0]);
		x[1] = LIVOX_TILE_WIDTH * (stroke->pos[0] + stroke->size[0]);
		y[0] = LIVOX_TILE_WIDTH * (stroke->pos[1]);
		y[1] = LIVOX_TILE_WIDTH * (stroke->pos[1] + stroke->size[1]);
		z[0] = LIVOX_TILE_WIDTH * (stroke->pos[2]);
		z[1] = LIVOX_TILE_WIDTH * (stroke->pos[2] + stroke->size[2]);
		glVertex3f (x[0], y[0], z[0]);
		glVertex3f (x[1], y[0], z[0]);
		glVertex3f (x[1], y[0], z[0]);
		glVertex3f (x[1], y[0], z[1]);
		glVertex3f (x[1], y[0], z[1]);
		glVertex3f (x[0], y[0], z[1]);
		glVertex3f (x[0], y[0], z[1]);
		glVertex3f (x[0], y[0], z[0]);

		glVertex3f (x[0], y[1], z[0]);
		glVertex3f (x[1], y[1], z[0]);
		glVertex3f (x[1], y[1], z[0]);
		glVertex3f (x[1], y[1], z[1]);
		glVertex3f (x[1], y[1], z[1]);
		glVertex3f (x[0], y[1], z[1]);
		glVertex3f (x[0], y[1], z[1]);
		glVertex3f (x[0], y[1], z[0]);

		glVertex3f (x[0], y[0], z[0]);
		glVertex3f (x[0], y[1], z[0]);
		glVertex3f (x[1], y[0], z[0]);
		glVertex3f (x[1], y[1], z[0]);
		glVertex3f (x[0], y[0], z[1]);
		glVertex3f (x[0], y[1], z[1]);
		glVertex3f (x[1], y[0], z[1]);
		glVertex3f (x[1], y[1], z[1]);
	}
	glEnd ();
}

static void
private_populate_brushes (liextDialog* self)
{
	int i;
	char buffer[256];
	ligenBrush* brush;
	liwdgWidget* widget;
	liwdgWidget* group;

	/* Clear brush group. */
	group = self->group_brushes;
	liwdg_group_set_size (LIWDG_GROUP (group), 1, 0);

	for (i = 0 ; i < self->generator->generator->brushes.count ; i++)
	{
		brush = self->generator->generator->brushes.array[i];
		snprintf (buffer, 256, "%d. %s\n", i + 1, brush->name);

		/* Create brush widget. */
		widget = liwdg_label_new (LIWDG_WIDGET (self)->manager);
		if (widget == NULL)
			continue;
		liwdg_label_set_text (LIWDG_LABEL (widget), buffer);
		if (i == self->active_brush)
			liwdg_label_set_highlight (LIWDG_LABEL (widget), 1);
		liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0,
			private_brush_selected, self, NULL);

		/* Append to group. */
		if (!liwdg_group_insert_row (LIWDG_GROUP (group), 0))
		{
			liwdg_widget_free (widget);
			continue;
		}
		liwdg_group_set_child (LIWDG_GROUP (group), 0, 0, widget);
	}
}

static void
private_populate_rules (liextDialog* self)
{
	int i;
	char buffer[256];
	ligenBrush* brush;
	ligenRule* rule;
	liwdgWidget* widget;

	/* Clear rule group. */
	liwdg_group_set_size (LIWDG_GROUP (self->group_rules), 1, 0);

	/* Get active brush. */
	if (self->generator->generator->brushes.count <= self->active_brush)
		return;
	brush = self->generator->generator->brushes.array[self->active_brush];

	/* Populate rule list. */
	for (i = 0 ; i < brush->rules.count ; i++)
	{
		rule = brush->rules.array[i];
		snprintf (buffer, 256, "%d. %s", i + 1, rule->name);

		/* Create rule widget. */
		widget = liwdg_label_new (LIWDG_WIDGET (self)->manager);
		if (widget == NULL)
			continue;
		liwdg_label_set_text (LIWDG_LABEL (widget), buffer);
		if (i == self->active_rule)
			liwdg_label_set_highlight (LIWDG_LABEL (widget), 1);
		liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0,
			private_rule_selected, self, NULL);

		/* Append to group. */
		if (!liwdg_group_insert_row (LIWDG_GROUP (self->group_rules), 0))
		{
			liwdg_widget_free (widget);
			continue;
		}
		liwdg_group_set_child (LIWDG_GROUP (self->group_rules), 0, 0, widget);
	}
}

static void
private_populate_strokes (liextDialog* self)
{
	int i;
	char buffer[256];
	ligenBrush* brush;
	ligenRule* rule;
	ligenRulestroke* stroke;
	limatMatrix modelview;
	limatMatrix projection;
	liwdgWidget* widget;
	liwdgRect rect;

	/* Clear stroke group. */
	liwdg_group_set_size (LIWDG_GROUP (self->group_strokes), 1, 0);

	/* Clear preview scene. */
	ligen_generator_clear_scene (self->generator->generator);

	/* Get active rule. */
	if (self->generator->generator->brushes.count <= self->active_brush)
		return;
	brush = self->generator->generator->brushes.array[self->active_brush];
	if (brush->rules.count <= self->active_rule)
		return;
	rule = brush->rules.array[self->active_rule];
	if (self->active_stroke > rule->strokes.count)
		self->active_stroke = LI_MAX (0, rule->strokes.count);

	/* Populate stroke list. */
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		stroke = rule->strokes.array + i;
		snprintf (buffer, 256, "%d at {%d,%d,%d}",
			stroke->brush, stroke->pos[0], stroke->pos[1], stroke->pos[2]);

		/* Create stroke widget. */
		widget = liwdg_label_new (LIWDG_WIDGET (self)->manager);
		if (widget == NULL)
			continue;
		liwdg_label_set_text (LIWDG_LABEL (widget), buffer);
		if (i == self->active_stroke)
			liwdg_label_set_highlight (LIWDG_LABEL (widget), 1);
		liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0,
			private_stroke_selected, self, NULL);

		/* Append to group. */
		if (!liwdg_group_insert_row (LIWDG_GROUP (self->group_strokes), 0))
		{
			liwdg_widget_free (widget);
			continue;
		}
		liwdg_group_set_child (LIWDG_GROUP (self->group_strokes), 0, 0, widget);
	}

	/* Rebuild preview scene. */
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		stroke = rule->strokes.array + i;
		ligen_generator_insert_stroke (self->generator->generator, stroke->brush,
			LIEXT_PREVIEW_CENTER + stroke->pos[0],
			LIEXT_PREVIEW_CENTER + stroke->pos[1],
			LIEXT_PREVIEW_CENTER + stroke->pos[2]);
	}
	ligen_generator_insert_stroke (self->generator->generator, brush->id,
		LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER);
	ligen_generator_rebuild_scene (self->generator->generator);

	/* Setup preview transformation. */
	liwdg_widget_get_allocation (self->render_strokes, &rect);
	modelview = limat_matrix_lookat (
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f * brush->size[0])/* - 15*/,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f * brush->size[1]) + 45,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f * brush->size[2])/* - 30.0f*/,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f * brush->size[0]),
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f * brush->size[1]),
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f * brush->size[2]),
		0.0f, 0.0f, 1.0f);
	projection = limat_matrix_perspective (45.0f, rect.width / rect.height, 1.0f, 100.0f);
	liwdg_render_set_modelview (LIWDG_RENDER (self->render_strokes), &modelview);
	liwdg_render_set_projection (LIWDG_RENDER (self->render_strokes), &projection);
}

/** @} */
/** @} */
/** @} */
