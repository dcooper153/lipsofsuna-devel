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
#include "ext-brushes.h"
#include "ext-dialog-brush.h"
#include "ext-module.h"
#include "ext-preview.h"

static const void*
private_base ();

static int
private_init (liextBrushes* self,
              liwdgManager* manager);

static void
private_free (liextBrushes* self);

static int
private_event (liextBrushes* self,
               liwdgEvent*   event);

static int
private_add (liextBrushes* self,
             liwdgWidget*  widget);

static int
private_remove (liextBrushes* self,
                liwdgWidget*  widget);

static int
private_rename (liextBrushes* self,
                liwdgWidget*  widget);

static int
private_selected (liextBrushes* self,
                  liwdgWidget*  widget,
                  liwdgTreerow* row);

static void
private_move (liextBrushes* self,
              int           dx,
              int           dy,
              int           dz);

static int
private_move_up (liextBrushes* self,
                 liwdgWidget*  widget);

static int
private_move_down (liextBrushes* self,
                   liwdgWidget*  widget);

static int
private_move_left (liextBrushes* self,
                   liwdgWidget*  widget);

static int
private_move_right (liextBrushes* self,
                    liwdgWidget*  widget);

static void
private_populate (liextBrushes* self);

static void
private_rebuild_preview (liextBrushes* self);

static liextBrushesTreerow*
private_get_active (liextBrushes* self);

/****************************************************************************/

const liwdgClass liextBrushesType =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorBrushes", sizeof (liextBrushes),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_brushes_new (liwdgManager* manager,
                   liextModule*  module)
{
	liextBrushes* data;
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liextBrushesType);
	if (self == NULL)
		return NULL;
	data = LIEXT_BRUSHES (self);
	data->module = module;

	/* Initialize scene preview. */
	data->widgets.preview = liext_preview_new (manager, module->module);
	if (data->widgets.preview == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liwdg_widget_set_request (data->widgets.preview, 320, 240);
	liwdg_group_set_child (LIWDG_GROUP (data->widgets.group_view), 0, 2, data->widgets.preview);
	data->generator = LIEXT_PREVIEW (data->widgets.preview)->generator;

	/* Populate brush list. */
	private_populate (data);

	return self;
}

int
liext_brushes_insert_brush (liextBrushes* self,
                            ligenBrush*   brush)
{
	liextBrushesTreerow* row;
	liwdgTreerow* trow;

	trow = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));

	/* Create row. */
	row = calloc (1, sizeof (liextBrushesTreerow));
	if (row == NULL)
		return 0;
	row->brush = brush;
	row->rule = NULL;
	row->stroke = -1;
	if (!liwdg_treerow_append_row (trow, brush->name, row))
	{
		free (row);
		return 0;
	}

	/* Insert brush. */
	if (!ligen_generator_insert_brush (self->generator, brush))
	{
		liwdg_treerow_remove_row (trow, liwdg_treerow_get_row_count (trow) - 1);
		free (row);
		return 0;
	}

	return 1;
}

int
liext_brushes_save (liextBrushes* self)
{
	return ligen_generator_write_brushes (self->generator);
}

void
liext_brushes_reset (liextBrushes* self)
{
	liext_preview_replace_materials (LIEXT_PREVIEW (self->widgets.preview));
	private_populate (self);
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgGroupType;
}

static int
private_init (liextBrushes* self,
              liwdgManager* manager)
{
	int i;
	liwdgWidget* group_buttons;
	liwdgWidget* group_tree;
	liwdgWidget* group_name;
	liwdgWidget* widgets[] =
	{
		liwdg_group_new_with_size (manager, 4, 1),
		liwdg_group_new_with_size (manager, 1, 5),
		liwdg_group_new_with_size (manager, 2, 1),
		liwdg_group_new_with_size (manager, 1, 3),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_entry_new (manager),
		liwdg_label_new (manager),
		liwdg_tree_new (manager)
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
	group_name = widgets[++i];
	self->widgets.group_view = widgets[++i];
	self->widgets.button_move_up = widgets[++i];
	self->widgets.button_move_down = widgets[++i];
	self->widgets.button_move_left = widgets[++i];
	self->widgets.button_move_right = widgets[++i];
	self->widgets.button_add = widgets[++i];
	self->widgets.button_remove = widgets[++i];
	self->widgets.entry_name = widgets[++i];
	self->widgets.label_type = widgets[++i];
	self->widgets.tree = widgets[++i];

	/* Configure widgets. */
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_move_up), "↑");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_move_down), "↓");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_move_left), "←");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_move_right), "→");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_add), "Add");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_remove), "Remove");
	liwdg_widget_insert_callback (self->widgets.button_move_up, LIWDG_CALLBACK_PRESSED, 0, private_move_up, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_move_down, LIWDG_CALLBACK_PRESSED, 0, private_move_down, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_move_left, LIWDG_CALLBACK_PRESSED, 0, private_move_left, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_move_right, LIWDG_CALLBACK_PRESSED, 0, private_move_right, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_add, LIWDG_CALLBACK_PRESSED, 0, private_add, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_remove, LIWDG_CALLBACK_PRESSED, 0, private_remove, self, NULL);
	liwdg_widget_insert_callback (self->widgets.entry_name, LIWDG_CALLBACK_EDITED, 0, private_rename, self, NULL);
	liwdg_widget_insert_callback (self->widgets.tree, LIWDG_CALLBACK_PRESSED, 0, private_selected, self, NULL);

	/* Tree. */
	liwdg_group_set_row_expand (LIWDG_GROUP (group_tree), 2, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_tree), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 4, liwdg_label_new_with_text (manager, "Brushes"));
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 3, self->widgets.tree);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 1, self->widgets.button_add);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 0, self->widgets.button_remove);

	/* Strokes. */
	liwdg_group_set_col_expand (LIWDG_GROUP (group_name), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_name), 0, 0, self->widgets.label_type);
	liwdg_group_set_child (LIWDG_GROUP (group_name), 1, 0, self->widgets.entry_name);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_buttons), 0, 1);
	liwdg_group_set_homogeneous (LIWDG_GROUP (group_buttons), 1);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 0, 0, self->widgets.button_move_left);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 1, 0, self->widgets.button_move_down);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 2, 0, self->widgets.button_move_up);
	liwdg_group_set_child (LIWDG_GROUP (group_buttons), 3, 0, self->widgets.button_move_right);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_view), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self->widgets.group_view), 2, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_view), 0, 1, group_name);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_view), 0, 0, group_buttons);

	/* Pack self. */
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 1, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self), 0, 1);
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
private_free (liextBrushes* self)
{
	liwdg_tree_foreach (LIWDG_TREE (self->widgets.tree), free);
	if (self->widgets.dialog != NULL)
	{
		liwdg_manager_remove_window (LIWDG_WIDGET (self)->manager, self->widgets.dialog);
		liwdg_widget_free (self->widgets.dialog);
	}
}

static int
private_event (liextBrushes* self,
               liwdgEvent*   event)
{
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
		if (self->widgets.dialog != NULL && !liwdg_widget_get_visible (self->widgets.dialog))
		{
			liwdg_manager_remove_window (LIWDG_WIDGET (self)->manager, self->widgets.dialog);
			liwdg_widget_free (self->widgets.dialog);
			self->widgets.dialog = NULL;
		}
	}

	return liwdgGroupType.event (LIWDG_WIDGET (self), event);
}

static int
private_add (liextBrushes* self,
             liwdgWidget*  widget)
{
	ligenRule* rule;
	liextBrushesTreerow* row;
	liextBrushesTreerow* row1;
	liwdgTreerow* trow;

	/* Get active item. */
	row = private_get_active (self);
	trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));

	/* Brush? */
	if (row == NULL || row->brush == NULL)
	{
		self->widgets.dialog = liext_dialog_brush_new (widget->manager, self->module);
		if (!liwdg_manager_insert_window (widget->manager, self->widgets.dialog))
		{
			liwdg_widget_free (self->widgets.dialog);
			self->widgets.dialog = NULL;
			return 0;
		}
		liwdg_widget_set_visible (self->widgets.dialog, 1);
	}

	/* Rule? */
	else if (row->rule == NULL)
	{
		/* Create rule. */
		rule = ligen_rule_new ();
		if (rule == NULL)
			return 0;
		if (!ligen_brush_insert_rule (row->brush, rule))
		{
			ligen_rule_free (rule);
			return 0;
		}

		/* Create row. */
		row1 = calloc (1, sizeof (liextBrushesTreerow));
		if (row1 == NULL)
			return 0;
		row1->brush = row->brush;
		row1->rule = rule;
		row1->stroke = -1;
		if (!liwdg_treerow_append_row (trow, rule->name, row1))
		{
			free (row1);
			return 0;
		}
	}

	/* Stroke? */
	else
	{
		/* Create stroke. */
		if (!ligen_rule_insert_stroke (row->rule, 0, 0, 0, 0, 0))
			return 0;

		/* Create row. */
		row1 = calloc (1, sizeof (liextBrushesTreerow));
		if (row1 == NULL)
			return 0;
		row1->brush = row->brush;
		row1->rule = row->rule;
		row1->stroke = row->rule->strokes.count - 1;
		if (!liwdg_treerow_append_row (trow, row->brush->name, row1))
		{
			free (row1);
			return 0;
		}
	}

	return 0;
}

static int
private_remove (liextBrushes* self,
                liwdgWidget*  widget)
{
	int c[3];
	int i[3];
	ligenRulestroke* stroke;
	liwdgTreerow* trow;
	liwdgTreerow* trows[4];
	liextBrushesTreerow* row;
	liextBrushesTreerow* row0;
	liextBrushesTreerow* row1;

	/* Get active item. */
	row = private_get_active (self);
	if (row == NULL || row->brush == NULL)
		return 0;

	/* Remove the selected item. */
	if (row->rule == NULL)
	{
		/* Remove invalid strokes. */
		trows[0] = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
		c[0] = liwdg_treerow_get_row_count (trows[0]);
		for (i[0] = 0 ; i[0] < c[0] ; i[0]++)
		{
			trows[1] = liwdg_treerow_get_row (trows[0], i[0]);
			c[1] = liwdg_treerow_get_row_count (trows[1]);
			for (i[1] = 0 ; i[1] < c[1] ; i[1]++)
			{
				trows[2] = liwdg_treerow_get_row (trows[1], i[1]);
				row0 = liwdg_treerow_get_data (trows[2]);
				c[2] = liwdg_treerow_get_row_count (trows[2]);
				for (i[2] = 0 ; i[2] < c[2] ; i[2]++)
				{
					trows[3] = liwdg_treerow_get_row (trows[2], i[2]);
					row1 = liwdg_treerow_get_data (trows[3]);
					stroke = row1->rule->strokes.array + row1->stroke;
					if (stroke->brush == row->brush->id)
					{
						liwdg_treerow_remove_row (trows[2], i[2]);
						free (row1);
						i[2]--;
						c[2]--;
					}
				}
			}
		}

		/* Remove the brush. */
		ligen_generator_remove_brush (self->generator, row->brush->id);
	}
	else if (row->stroke < 0)
		ligen_brush_remove_rule (row->brush, row->rule->id);
	else
		ligen_rule_remove_stroke (row->rule, row->stroke);

	/* Remove the row. */
	trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	free (liwdg_treerow_get_data (trow));
	liwdg_treerow_remove_row (
		liwdg_treerow_get_parent (trow),
		liwdg_treerow_get_index (trow));

	return 0;
}

static int
private_rename (liextBrushes* self,
                liwdgWidget*  widget)
{
	int c[3];
	int i[3];
	const char* name;
	lialgU32dicIter iter;
	liextBrushesTreerow* row;
	liextBrushesTreerow* row1;
	ligenBrush* brush;
	ligenRulestroke* stroke;
	liwdgTreerow* trow;
	liwdgTreerow* trows[4];

	row = private_get_active (self);
	if (row == NULL || row->brush == NULL)
		return 0;

	/* Brush? */
	if (row->rule == NULL)
	{
		/* Replace brush name. */
		name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
		trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
		ligen_brush_set_name (row->brush, name);
		liwdg_treerow_set_text (trow, name);

		/* Update stroke labels. */
		trows[0] = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
		c[0] = liwdg_treerow_get_row_count (trows[0]);
		for (i[0] = 0 ; i[0] < c[0] ; i[0]++)
		{
			trows[1] = liwdg_treerow_get_row (trows[0], i[0]);
			c[1] = liwdg_treerow_get_row_count (trows[1]);
			for (i[1] = 0 ; i[1] < c[1] ; i[1]++)
			{
				trows[2] = liwdg_treerow_get_row (trows[1], i[1]);
				c[2] = liwdg_treerow_get_row_count (trows[2]);
				for (i[2] = 0 ; i[2] < c[2] ; i[2]++)
				{
					trows[3] = liwdg_treerow_get_row (trows[2], i[2]);
					row1 = liwdg_treerow_get_data (trows[3]);
					stroke = row1->rule->strokes.array + row1->stroke;
					if (stroke->brush == row->brush->id)
						liwdg_treerow_set_text (trows[3], name);
				}
			}
		}
	}

	/* Rule? */
	else if (row->stroke < 0)
	{
		name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
		trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
		ligen_rule_set_name (row->rule, name);
		liwdg_treerow_set_text (trow, name);
	}

	/* Stroke? */
	else
	{
		name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
		LI_FOREACH_U32DIC (iter, self->generator->brushes)
		{
			brush = iter.value;
			if (!strcmp (name, row->brush->name))
			{
				trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
				row->rule->strokes.array[row->stroke].brush = brush->id;
				liwdg_treerow_set_text (trow, name);
				break;
			}
		}
	}

	return 0;
}

static int
private_selected (liextBrushes* self,
                  liwdgWidget*  widget,
                  liwdgTreerow* row)
{
	liextBrushesTreerow* data;
	ligenBrush* brush;
	ligenRulestroke* stroke;
	liwdgTreerow* row0;

	/* Deselect old. */
	row0 = liwdg_tree_get_active (LIWDG_TREE (widget));
	if (row0 != NULL)
		liwdg_treerow_set_highlighted (row0, 0);

	/* Select new. */
	liwdg_treerow_set_highlighted (row, 1);
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);

	/* Set info. */
	if (data == NULL || data->brush == NULL)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), "");
	}
	else if (data->rule == NULL)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Brush:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), data->brush->name);
	}
	else if (data->stroke < 0)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Rule:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), data->rule->name);
	}
	else
	{
		stroke = data->rule->strokes.array + data->stroke;
		brush = ligen_generator_find_brush (self->generator, stroke->brush);
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Stroke:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), brush != NULL? brush->name : "");
	}

	/* Rebuild preview. */
	private_rebuild_preview (self);

	return 0;
}

static void
private_move (liextBrushes* self,
              int           dx,
              int           dy,
              int           dz)
{
	liextBrushesTreerow* row;
	ligenRulestroke* stroke;

	/* Get active stroke. */
	row = private_get_active (self);
	if (row == NULL || row->stroke < 0)
		return;

	/* Move stroke. */
	stroke = row->rule->strokes.array + row->stroke,
	stroke->pos[0] += dx;
	stroke->pos[1] += dy;
	stroke->pos[2] += dz;
	private_rebuild_preview (self);

	return;
}

static int
private_move_up (liextBrushes* self,
                 liwdgWidget*  widget)
{
	private_move (self, 0, 0, 1);
	return 0;
}

static int
private_move_down (liextBrushes* self,
                   liwdgWidget*  widget)
{
	private_move (self, 0, 0, -1);
	return 0;
}

static int
private_move_left (liextBrushes* self,
                   liwdgWidget*  widget)
{
	private_move (self, 1, 0, 0);
	return 0;
}

static int
private_move_right (liextBrushes* self,
                    liwdgWidget*  widget)
{
	private_move (self, -1, 0, 0);
	return 0;
}

static void
private_populate (liextBrushes* self)
{
	int j;
	int k;
	lialgU32dicIter iter;
	liextBrushesTreerow* tmp;
	ligenBrush* brush;
	ligenBrush* brush1;
	ligenRule* rule;
	ligenRulestroke* stroke;
	liwdgTreerow* tree;
	liwdgTreerow* row0;
	liwdgTreerow* row1;

	/* Clear tree. */
	tree = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
	liwdg_tree_foreach (LIWDG_TREE (self->widgets.tree), free);
	liwdg_tree_clear (LIWDG_TREE (self->widgets.tree));

	/* Add root info. */
	tmp = calloc (1, sizeof (liextBrushesTreerow));
	tmp->brush = NULL;
	tmp->rule = NULL;
	tmp->stroke = -1;
	liwdg_treerow_append_row (tree, "Root", tmp);

	/* Loop through brushes. */
	LI_FOREACH_U32DIC (iter, self->generator->brushes)
	{
		brush = iter.value;

		/* Add brush info. */
		tmp = calloc (1, sizeof (liextBrushesTreerow));
		tmp->brush = brush;
		tmp->rule = NULL;
		tmp->stroke = -1;
		row0 = liwdg_treerow_append_row (tree, brush->name, tmp);
		if (row0 == NULL)
			return;

		/* Loop through rules. */
		for (j = 0 ; j < brush->rules.count ; j++)
		{
			rule = brush->rules.array[j];

			/* Add rule info. */
			tmp = calloc (1, sizeof (liextBrushesTreerow));
			tmp->brush = brush;
			tmp->rule = rule;
			tmp->stroke = -1;
			row1 = liwdg_treerow_append_row (row0, rule->name, tmp);
			if (row1 == NULL)
				return;

			for (k = 0 ; k < rule->strokes.count ; k++)
			{
				stroke = rule->strokes.array + k;
				brush1 = ligen_generator_find_brush (self->generator, stroke->brush);
				assert (brush1 != NULL);

				/* Add stroke info. */
				tmp = calloc (1, sizeof (liextBrushesTreerow));
				tmp->brush = brush;
				tmp->rule = rule;
				tmp->stroke = k;
				liwdg_treerow_append_row (row1, brush1->name, tmp);
			}
		}
	}
}

static void
private_rebuild_preview (liextBrushes* self)
{
	int i;
	liextBrushesTreerow* row;
	ligenRulestroke* stroke;

	row = private_get_active (self);
	if (row == NULL || row->brush == NULL)
	{
		liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
		liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
		return;
	}

	/* Brush? */
	if (row->rule == NULL)
	{
		liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
		liext_preview_insert_stroke (LIEXT_PREVIEW (self->widgets.preview), 0, 0, 0, row->brush->id);
		liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
	}

	/* Rule? */
	else
	{
		liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
		for (i = 0 ; i < row->rule->strokes.count ; i++)
		{
			stroke = row->rule->strokes.array + i;
			liext_preview_insert_stroke (LIEXT_PREVIEW (self->widgets.preview),
				stroke->pos[0], stroke->pos[1], stroke->pos[2], stroke->brush);
		}
		liext_preview_insert_stroke (LIEXT_PREVIEW (self->widgets.preview), 0, 0, 0, row->brush->id);
		liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
	}
}

static liextBrushesTreerow*
private_get_active (liextBrushes* self)
{
	liwdgTreerow* row;

	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return NULL;
	return liwdg_treerow_get_data (row);
}

/** @} */
/** @} */
/** @} */
