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
private_generate (liextDialog* self);

static int
private_insert (liextDialog* self);

static int
private_remove (liextDialog* self);

static void
private_render_preview (liwdgWidget* widget,
                        liextDialog* self);

static int
private_brush_selected (liextDialog* self,
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
	int i;
	ligenBrush* brush;
	liwdgWidget* self;
	liwdgWidget* child;
	liwdgWidget* group;

	self = liwdg_widget_new (manager, &liextDialogType);
	if (self == NULL)
		return NULL;
	LIEXT_DIALOG (self)->generator = generator;
	LIEXT_DIALOG (self)->module = generator->module;
	group = LIEXT_DIALOG (self)->group_brushes;

	/* Pack brushes. */
	for (i = 0 ; i < generator->generator->brushes.count ; i++)
	{
		brush = generator->generator->brushes.array[i];
		if (liwdg_group_insert_row (LIWDG_GROUP (group), 0))
		{
			child = liwdg_button_new (manager);
			if (child != NULL)
			{
				char buffer[256];
				snprintf (buffer, 256, "%d. %s\n", i + 1, brush->name);
				liwdg_button_set_text (LIWDG_BUTTON (child), buffer);
				liwdg_group_set_child (LIWDG_GROUP (group), 0, 0, child);
				liwdg_widget_insert_callback (child, LIWDG_CALLBACK_PRESSED, 0,
					private_brush_selected, self, NULL);
			}
		}
	}

	/* Initialize preview widget. */
	LIEXT_DIALOG (self)->render_strokes = liwdg_render_new (manager, generator->scene);
	if (LIEXT_DIALOG (self)->render_strokes == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 2, LIEXT_DIALOG (self)->render_strokes);
	LIWDG_RENDER (LIEXT_DIALOG (self)->render_strokes)->custom_render_func = (void*) private_render_preview;
	LIWDG_RENDER (LIEXT_DIALOG (self)->render_strokes)->custom_render_data = self;
	liwdg_widget_set_request (LIEXT_DIALOG (self)->render_strokes, 240, 160);

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
	liwdgWidget* widgets[] =
	{
		liwdg_group_new_with_size (manager, 1, 0),
		liwdg_group_new_with_size (manager, 1, 0),
		liwdg_label_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),

		liwdg_label_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 1, 7))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	self->group_brushes = widgets[(i = 0)];
	self->group_strokes = widgets[++i];
	self->label_brush = widgets[++i];
	self->button_move_up = widgets[++i];
	self->button_move_down = widgets[++i];
	self->button_move_left = widgets[++i];
	self->button_move_right = widgets[++i];

	self->label_rule = widgets[++i];
	self->button_insert = widgets[++i];
	self->button_remove = widgets[++i];
	self->button_generate = widgets[++i];

	/* Configure widgets. */
	liwdg_group_set_spacings (LIWDG_GROUP (self->group_brushes), 0, 0);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->group_brushes), 0, 1);
	liwdg_group_set_spacings (LIWDG_GROUP (self->group_strokes), 0, 0);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->group_strokes), 0, 1);
	liwdg_button_set_text (LIWDG_BUTTON (self->button_move_up), "↑");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_move_down), "↓");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_move_left), "←");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_move_right), "→");
	liwdg_widget_insert_callback (self->button_move_up, LIWDG_CALLBACK_PRESSED, 0, private_move_up, self, NULL);
	liwdg_widget_insert_callback (self->button_move_down, LIWDG_CALLBACK_PRESSED, 0, private_move_down, self, NULL);
	liwdg_widget_insert_callback (self->button_move_left, LIWDG_CALLBACK_PRESSED, 0, private_move_left, self, NULL);
	liwdg_widget_insert_callback (self->button_move_right, LIWDG_CALLBACK_PRESSED, 0, private_move_right, self, NULL);

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
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 6, self->group_brushes);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 5, self->label_brush);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 4, self->group_strokes);

	liwdgWidget* group = liwdg_group_new_with_size (manager, 4, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (group), 0, 1);
	liwdg_group_set_homogeneous (LIWDG_GROUP (group), 1);
	liwdg_group_set_child (LIWDG_GROUP (group), 0, 0, self->button_move_left);
	liwdg_group_set_child (LIWDG_GROUP (group), 1, 0, self->button_move_down);
	liwdg_group_set_child (LIWDG_GROUP (group), 2, 0, self->button_move_up);
	liwdg_group_set_child (LIWDG_GROUP (group), 3, 0, self->button_move_right);

	liwdg_group_set_child (LIWDG_GROUP (self), 0, 3, group);

/*	liwdg_group_set_child (LIWDG_GROUP (self), 0, 3, self->label_rule);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 2, self->button_insert);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 1, self->button_remove);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->button_generate);*/

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
		x[0] = stroke->pos[0];
		x[1] = stroke->pos[0] + stroke->size[0];
		y[0] = stroke->pos[1];
		y[1] = stroke->pos[1] + stroke->size[1];
		z[0] = stroke->pos[2];
		z[1] = stroke->pos[2] + stroke->size[2];
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

static int
private_brush_selected (liextDialog* self,
                        liwdgWidget* widget)
{
	int i;
	int h;
	liwdgWidget* child;

	liwdg_group_get_size (LIWDG_GROUP (self->group_brushes), NULL, &h);
	for (i = 0 ; i < h ; i++)
	{
		child = liwdg_group_get_child (LIWDG_GROUP (self->group_brushes), 0, i);
		if (child == widget)
		{
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
private_stroke_selected (liextDialog* self,
                         liwdgWidget* widget)
{
	int i;
	int h;
	liwdgWidget* child;

	liwdg_group_get_size (LIWDG_GROUP (self->group_strokes), NULL, &h);
	for (i = 0 ; i < h ; i++)
	{
		child = liwdg_group_get_child (LIWDG_GROUP (self->group_strokes), 0, i);
		if (child == widget)
		{
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
private_populate_rules (liextDialog* self)
{
	char buffer[256];
	ligenBrush* brush;

	/* Clear brush text. */
	liwdg_label_set_text (LIWDG_LABEL (self->label_brush), "");

	/* Get active brush. */
	if (self->generator->generator->brushes.count <= self->active_brush)
		return;
	brush = self->generator->generator->brushes.array[self->active_brush];

	/* Set new brush text. */
	snprintf (buffer, 256, "Brush #%d: %s\n%d rules", brush->id + 1, brush->name, brush->rules.count);
	liwdg_label_set_text (LIWDG_LABEL (self->label_brush), buffer);
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

	/* Populate stroke list and preview scene. */
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		stroke = rule->strokes.array + i;
		snprintf (buffer, 256, "%d at {%d,%d,%d}",
			stroke->brush, stroke->pos[0], stroke->pos[1], stroke->pos[2]);

		/* Create stroke widget. */
		widget = liwdg_button_new (LIWDG_WIDGET (self)->manager);
		if (widget == NULL)
			continue;
		liwdg_button_set_text (LIWDG_BUTTON (widget), buffer);
		liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0,
			private_stroke_selected, self, NULL);

		/* Append to group. */
		if (!liwdg_group_append_row (LIWDG_GROUP (self->group_strokes)))
		{
			liwdg_widget_free (widget);
			continue;
		}
		liwdg_group_set_child (LIWDG_GROUP (self->group_strokes), 0, i, widget);
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
		LIEXT_PREVIEW_CENTER + 0.5f * brush->size[0]/* - 15*/,
		LIEXT_PREVIEW_CENTER + 0.5f * brush->size[1] + 45,
		LIEXT_PREVIEW_CENTER + 0.5f * brush->size[2]/* - 30.0f*/,
		LIEXT_PREVIEW_CENTER + 0.5f * brush->size[0],
		LIEXT_PREVIEW_CENTER + 0.5f * brush->size[1],
		LIEXT_PREVIEW_CENTER + 0.5f * brush->size[2],
		0.0f, 0.0f, 1.0f);
	projection = limat_matrix_perspective (45.0f, rect.width / rect.height, 1.0f, 100.0f);
	liwdg_render_set_modelview (LIWDG_RENDER (self->render_strokes), &modelview);
	liwdg_render_set_projection (LIWDG_RENDER (self->render_strokes), &projection);
}

/** @} */
/** @} */
/** @} */
