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
#include "ext-dialog-brush.h"
#include "ext-preview.h"

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
private_add (liextDialog* self,
             liwdgWidget* widget);

static int
private_edit (liextDialog* self,
              liwdgWidget* widget);

static int
private_remove (liextDialog* self,
                liwdgWidget* widget);

static int
private_selected (liextDialog* self,
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
private_populate (liextDialog* self);

static void
private_rebuild_preview (liextDialog* self);

static int
private_get_active (liextDialog*      self,
                    ligenBrush**      brush,
                    ligenRule**       rule,
                    ligenRulestroke** stroke);

/****************************************************************************/

const liwdgClass liextDialogType =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorDialog", sizeof (liextDialog),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_dialog_new (liwdgManager* manager,
                  liextModule*  module)
{
	liextDialog* data;
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liextDialogType);
	if (self == NULL)
		return NULL;
	data = LIEXT_DIALOG (self);
	data->module = module;

	/* Initialize scene preview. */
	data->widgets.preview = liext_preview_new (manager, module->module);
	if (data->widgets.preview == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liwdg_widget_set_request (data->widgets.preview, 320, 240);
	liwdg_group_set_child (LIWDG_GROUP (data->widgets.group_view), 0, 1, data->widgets.preview);
	data->generator = LIEXT_PREVIEW (data->widgets.preview)->generator;

	/* Populate brush list. */
	private_populate (LIEXT_DIALOG (self));

	return self;
}

int
liext_dialog_save (liextDialog* self)
{
	return ligen_generator_write_brushes (self->generator);
}

void
liext_dialog_update (liextDialog* self)
{
	private_populate (self);
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
	liwdgWidget* group_tree;
	liwdgWidget* widgets[] =
	{
		liwdg_group_new_with_size (manager, 4, 1),
		liwdg_group_new_with_size (manager, 1, 6),
		liwdg_group_new_with_size (manager, 1, 2),
		liwdg_group_new_with_size (manager, 1, 0),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 2, 1))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	group_buttons = widgets[(i = 0)];
	group_tree = widgets[++i];
	self->widgets.group_view = widgets[++i];
	self->widgets.group_tree = widgets[++i];
	self->widgets.button_move_up = widgets[++i];
	self->widgets.button_move_down = widgets[++i];
	self->widgets.button_move_left = widgets[++i];
	self->widgets.button_move_right = widgets[++i];
	self->widgets.button_add = widgets[++i];
	self->widgets.button_edit = widgets[++i];
	self->widgets.button_remove = widgets[++i];

	/* Configure widgets. */
	liwdg_group_set_spacings (LIWDG_GROUP (self->widgets.group_tree), 0, 0);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_tree), 0, 1);
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_move_up), "↑");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_move_down), "↓");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_move_left), "←");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_move_right), "→");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_add), "Add");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_edit), "Edit");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_remove), "Remove");
	liwdg_widget_insert_callback (self->widgets.button_move_up, LIWDG_CALLBACK_PRESSED, 0, private_move_up, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_move_down, LIWDG_CALLBACK_PRESSED, 0, private_move_down, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_move_left, LIWDG_CALLBACK_PRESSED, 0, private_move_left, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_move_right, LIWDG_CALLBACK_PRESSED, 0, private_move_right, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_add, LIWDG_CALLBACK_PRESSED, 0, private_add, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_edit, LIWDG_CALLBACK_PRESSED, 0, private_edit, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_remove, LIWDG_CALLBACK_PRESSED, 0, private_remove, self, NULL);

	/* Tree. */
	liwdg_group_set_row_expand (LIWDG_GROUP (group_tree), 3, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_tree), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 5, liwdg_label_new_with_text (manager, "Brushes"));
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 4, self->widgets.group_tree);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 2, self->widgets.button_add);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 1, self->widgets.button_edit);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 0, self->widgets.button_remove);

	/* Strokes. */
	liwdg_group_set_col_expand (LIWDG_GROUP (group_buttons), 0, 1);
	liwdg_group_set_homogeneous (LIWDG_GROUP (group_buttons), 1);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 0, 0, self->widgets.button_move_left);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 1, 0, self->widgets.button_move_down);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 2, 0, self->widgets.button_move_up);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 3, 0, self->widgets.button_move_right);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_view), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self->widgets.group_view), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_view), 0, 0, group_buttons);

	/* Pack self. */
	liwdg_window_set_title (LIWDG_WINDOW (self), "Generator");
	liwdg_group_set_margins (LIWDG_GROUP (self), 5, 5, 5, 5);
	liwdg_group_set_spacings (LIWDG_GROUP (self), 5, 5);
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, group_tree);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 0, self->widgets.group_view);

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
	if (self->widgets.dialog != NULL)
	{
		liwdg_manager_remove_window (LIWDG_WIDGET (self)->manager, self->widgets.dialog);
		liwdg_widget_free (self->widgets.dialog);
	}
	free (self->rows.array);
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
		if (self->widgets.dialog != NULL && !liwdg_widget_get_visible (self->widgets.dialog))
		{
			liwdg_manager_remove_window (LIWDG_WIDGET (self)->manager, self->widgets.dialog);
			liwdg_widget_free (self->widgets.dialog);
			self->widgets.dialog = NULL;
		}
	}

	return liwdgWindowType.event (LIWDG_WIDGET (self), event);
}

static int
private_add (liextDialog* self,
             liwdgWidget* widget)
{
	ligenBrush* brush;
	ligenRule* rule;
	ligenRulestroke* stroke;

	/* Get active item. */
	private_get_active (self, &brush, &rule, &stroke);

	/* Brush? */
	if (brush == NULL)
	{
		self->widgets.dialog = liext_dialog_brush_new (widget->manager, self->module);
		if (!liwdg_manager_insert_window (widget->manager, self->widgets.dialog))
		{
			liwdg_widget_free (self->widgets.dialog);
			self->widgets.dialog = NULL;
			return 0;
		}
		liwdg_widget_set_visible (self->widgets.dialog, 1);
		return 0;
	}

	/* Rule? */
	if (rule == NULL)
	{
		rule = ligen_rule_new ();
		if (rule == NULL)
			return 0;
		if (!ligen_brush_insert_rule (brush, rule))
		{
			ligen_rule_free (rule);
			return 0;
		}
		private_populate (self);
		return 0;
	}

	/* Stroke? */
	if (1)
	{
		ligen_rule_insert_stroke (rule, 0, 0, 0, 0, 0);
		private_populate (self);
		return 0;
	}

	return 0;
}

static int
private_edit (liextDialog* self,
              liwdgWidget* widget)
{
	int i;
	ligenBrush* brush;
	ligenRule* rule;
	ligenRulestroke* stroke;

	/* Get active stroke. */
	if (private_get_active (self, &brush, &rule, &stroke) < 3)
		return 0;

	/* FIXME: Popup a proper dialog. */
	for (i = 0 ; i < self->rows.count ; i++)
	{
		if (self->rows.array[i].brush == stroke->brush)
			break;
	}
	for (i++ ; 1 ; i++)
	{
		if (i >= self->rows.count)
			i = 0;
		if (self->rows.array[i].brush != -1 &&
		    self->rows.array[i].rule == -1)
		{
			stroke->brush = self->rows.array[i].brush;
			private_populate (self);
			break;
		}
	}

	return 0;
}

static int
private_remove (liextDialog* self,
                liwdgWidget* widget)
{
	ligenBrush* brush;
	ligenRule* rule;
	ligenRulestroke* stroke;

	/* Get active item. */
	if (private_get_active (self, &brush, &rule, &stroke) < 1)
		return 0;

	/* Remove the selected item. */
	if (stroke != NULL)
		ligen_rule_remove_stroke (rule, self->rows.array[self->active_row].stroke);
	else if (rule != NULL)
		ligen_brush_remove_rule (brush, rule->id);
	else
		ligen_generator_remove_brush (self->generator, brush->id);
	private_populate (self);

	return 0;
}

static int
private_selected (liextDialog* self,
                  liwdgWidget* widget)
{
	int i;

	/* Set active row. */
	for (i = 0 ; i < self->rows.count ; i++)
	{
		if (self->rows.array[i].widget == widget)
		{
			if (self->active_row < self->rows.count)
				liwdg_label_set_highlight (LIWDG_LABEL (self->rows.array[self->active_row].widget), 0);
			liwdg_label_set_highlight (LIWDG_LABEL (widget), 1);
			self->active_row = i;
			break;
		}
	}

	/* Rebuild preview. */
	private_rebuild_preview (self);

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

	/* Get active stroke. */
	if (private_get_active (self, &brush, &rule, &stroke) < 3)
		return;

	/* Move stroke. */
	stroke->pos[0] += dx;
	stroke->pos[1] += dy;
	stroke->pos[2] += dz;
	private_populate (self);

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
private_populate (liextDialog* self)
{
	int i;
	int j;
	int k;
	char buffer[256];
	lialgU32dicIter iter;
	liextDialogTreerow tmp;
	ligenBrush* brush;
	ligenBrush* brush1;
	ligenRule* rule;
	ligenRulestroke* stroke;
	liwdgWidget* group;
	liwdgWidget* widget;

	/* Clear tree. */
	free (self->rows.array);
	self->rows.array = NULL;
	self->rows.count = 0;
	group = self->widgets.group_tree;
	liwdg_group_set_size (LIWDG_GROUP (group), 1, 0);

	/* Add root label. */
	widget = liwdg_label_new (LIWDG_WIDGET (self)->manager);
	if (widget != NULL)
	{
		liwdg_label_set_text (LIWDG_LABEL (widget), "Root");
		liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0, private_selected, self, NULL);
		if (liwdg_group_insert_row (LIWDG_GROUP (group), 0))
			liwdg_group_set_child (LIWDG_GROUP (group), 0, 0, widget);
		else
			liwdg_widget_free (widget);
	}

	/* Add root info. */
	tmp.brush = -1;
	tmp.rule = -1;
	tmp.stroke = -1;
	tmp.widget = widget;
	lialg_array_append (&self->rows, &tmp);

	/* Loop through brushes. */
	i = 0;
	LI_FOREACH_U32DIC (iter, self->generator->brushes)
	{
		brush = iter.value;

		/* Add brush label. */
		widget = liwdg_label_new (LIWDG_WIDGET (self)->manager);
		if (widget != NULL)
		{
			snprintf (buffer, 256, "| %s\n", brush->name);
			liwdg_label_set_text (LIWDG_LABEL (widget), buffer);
			liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0, private_selected, self, NULL);
			if (liwdg_group_insert_row (LIWDG_GROUP (group), 0))
				liwdg_group_set_child (LIWDG_GROUP (group), 0, 0, widget);
			else
				liwdg_widget_free (widget);
		}

		/* Add brush info. */
		tmp.brush = brush->id;
		tmp.rule = -1;
		tmp.stroke = -1;
		tmp.widget = widget;
		lialg_array_append (&self->rows, &tmp);

		/* Loop through rules. */
		for (j = 0 ; j < brush->rules.count ; j++)
		{
			rule = brush->rules.array[j];

			/* Add rule label. */
			widget = liwdg_label_new (LIWDG_WIDGET (self)->manager);
			if (widget != NULL)
			{
				snprintf (buffer, 256, "| | %s\n", rule->name);
				liwdg_label_set_text (LIWDG_LABEL (widget), buffer);
				liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0, private_selected, self, NULL);
				if (liwdg_group_insert_row (LIWDG_GROUP (group), 0))
					liwdg_group_set_child (LIWDG_GROUP (group), 0, 0, widget);
				else
					liwdg_widget_free (widget);
			}

			/* Add rule info. */
			tmp.brush = brush->id;
			tmp.rule = rule->id;
			tmp.stroke = -1;
			tmp.widget = widget;
			lialg_array_append (&self->rows, &tmp);

			for (k = 0 ; k < rule->strokes.count ; k++)
			{
				stroke = rule->strokes.array + k;
				brush1 = ligen_generator_find_brush (self->generator, stroke->brush);
				assert (brush1 != NULL);

				/* Add stroke label. */
				widget = liwdg_label_new (LIWDG_WIDGET (self)->manager);
				if (widget != NULL)
				{
					snprintf (buffer, 256, "| | | %s\n", brush1->name);
					liwdg_label_set_text (LIWDG_LABEL (widget), buffer);
					liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0, private_selected, self, NULL);
					if (liwdg_group_insert_row (LIWDG_GROUP (group), 0))
						liwdg_group_set_child (LIWDG_GROUP (group), 0, 0, widget);
					else
						liwdg_widget_free (widget);
				}

				/* Add stroke info. */
				tmp.brush = brush->id;
				tmp.rule = j;
				tmp.stroke = k;
				tmp.widget = widget;
				lialg_array_append (&self->rows, &tmp);
			}
		}

		i++;
	}

	/* Highlight active row. */
	if (self->active_row < self->rows.count)
		private_selected (self, self->rows.array[self->active_row].widget);
	else
		private_selected (self, NULL);
}

static void
private_rebuild_preview (liextDialog* self)
{
	int i;
	liextDialogTreerow* row;
	ligenBrush* brush;
	ligenRule* rule;
	ligenRulestroke* stroke;

	if (self->active_row < self->rows.count)
	{
		row = self->rows.array + self->active_row;
		if (row->rule >= 0)
		{
			/* Rule preview. */
			brush = ligen_generator_find_brush (self->generator, row->brush);
			assert (brush != NULL);
			rule = brush->rules.array[row->rule];
			liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
			for (i = 0 ; i < rule->strokes.count ; i++)
			{
				stroke = rule->strokes.array + i;
				liext_preview_insert_stroke (LIEXT_PREVIEW (self->widgets.preview),
					stroke->pos[0], stroke->pos[1], stroke->pos[2], stroke->brush);
			}
			liext_preview_insert_stroke (LIEXT_PREVIEW (self->widgets.preview), 0, 0, 0, brush->id);
			liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
		}
		else if (row->brush >= 0)
		{
			/* Brush preview. */
			brush = ligen_generator_find_brush (self->generator, row->brush);
			assert (brush != NULL);
			liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
			liext_preview_insert_stroke (LIEXT_PREVIEW (self->widgets.preview), 0, 0, 0, brush->id);
			liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
		}
		else
		{
			liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
			liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
		}
	}
	else
	{
		liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
		liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
	}
}

static int
private_get_active (liextDialog*      self,
                    ligenBrush**      brush,
                    ligenRule**       rule,
                    ligenRulestroke** stroke)
{
	liextDialogTreerow* row;
	ligenBrush* brush_;
	ligenRule* rule_;
	ligenRulestroke* stroke_;

	/* Get active row. */
	if (self->active_row >= self->rows.count)
	{
		if (brush != NULL) *brush = NULL;
		if (rule != NULL) *rule = NULL;
		if (stroke != NULL) *stroke = NULL;
		return 0;
	}
	row = self->rows.array + self->active_row;

	/* Get active brush. */
	if (row->brush < 0)
	{
		if (brush != NULL) *brush = NULL;
		if (rule != NULL) *rule = NULL;
		if (stroke != NULL) *stroke = NULL;
		return 0;
	}
	brush_ = ligen_generator_find_brush (self->generator, row->brush);
	assert (brush_ != NULL);

	/* Get active rule. */
	if (row->rule < 0)
	{
		if (brush != NULL) *brush = brush_;
		if (rule != NULL) *rule = NULL;
		if (stroke != NULL) *stroke = NULL;
		return 1;
	}
	assert (row->rule < brush_->rules.count);
	rule_ = brush_->rules.array[row->rule];

	/* Get active stroke. */
	if (row->stroke < 0)
	{
		if (brush != NULL) *brush = brush_;
		if (rule != NULL) *rule = rule_;
		if (stroke != NULL) *stroke = NULL;
		return 2;
	}
	assert (row->stroke < rule_->strokes.count);
	stroke_ = rule_->strokes.array + row->stroke;
	if (brush != NULL) *brush = brush_;
	if (rule != NULL) *rule = rule_;
	if (stroke != NULL) *stroke = stroke_;

	return 3;
}

/** @} */
/** @} */
/** @} */
