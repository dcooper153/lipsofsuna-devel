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

enum
{
	LIEXT_BRUSHES_ROWTYPE_ROOT,
	LIEXT_BRUSHES_ROWTYPE_BRUSH,
	LIEXT_BRUSHES_ROWTYPE_OBJECTS,
	LIEXT_BRUSHES_ROWTYPE_OBJECT,
	LIEXT_BRUSHES_ROWTYPE_RULES,
	LIEXT_BRUSHES_ROWTYPE_RULE,
	LIEXT_BRUSHES_ROWTYPE_STROKE
};

typedef struct _liextBrushesTreerow liextBrushesTreerow;
struct _liextBrushesTreerow
{
	ligenBrush* brush;
	ligenRule* rule;
	int stroke;
	int type;
};

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

static liwdgTreerow*
private_append_brush (liextBrushes* self,
                      ligenBrush*   brush);

static liwdgTreerow*
private_append_rule (liextBrushes* self,
                     liwdgTreerow* row,
                     ligenRule*    rule);

static liwdgTreerow*
private_append_stroke (liextBrushes* self,
                       liwdgTreerow* row,
                       int           index);

static void
private_remove_strokes (liextBrushes* self,
                        liwdgTreerow* row,
                        int           brush);

static void
private_rename_strokes (liextBrushes* self,
                        liwdgTreerow* row,
                        int           brush,
                        const char*   text);

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
	/* Insert brush. */
	if (!ligen_generator_insert_brush (self->generator, brush))
		return 0;
	if (!private_append_brush (self, brush))
	{
		ligen_generator_remove_brush (self->generator, brush->id);
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
	liext_preview_replace_materials (LIEXT_PREVIEW (self->widgets.preview),
		self->module->module->voxels);
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
		liwdg_group_new_with_size (manager, 1, 4),
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
	liextBrushesTreerow* data;
	liwdgTreerow* row;

	/* Get active row. */
	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return 0;
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);

	/* Add context specific data. */
	switch (data->type)
	{
		case LIEXT_BRUSHES_ROWTYPE_ROOT:
			self->widgets.dialog = liext_dialog_brush_new (widget->manager, self->module);
			if (!liwdg_manager_insert_window (widget->manager, self->widgets.dialog))
			{
				liwdg_widget_free (self->widgets.dialog);
				self->widgets.dialog = NULL;
				return 0;
			}
			liwdg_widget_set_visible (self->widgets.dialog, 1);
			break;
		case LIEXT_BRUSHES_ROWTYPE_BRUSH:
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECTS:
			/* FIXME: Add an object. */
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECT:
			/* FIXME: Add an object. */
			break;
		case LIEXT_BRUSHES_ROWTYPE_RULES:
			rule = ligen_rule_new ();
			if (rule == NULL)
				return 0;
			if (!ligen_brush_insert_rule (data->brush, rule))
			{
				ligen_rule_free (rule);
				return 0;
			}
			private_append_rule (self, row, rule);
			break;
		case LIEXT_BRUSHES_ROWTYPE_RULE:
			if (!ligen_rule_insert_stroke (data->rule, 0, 0, 0, 0, 0))
				return 0;
			private_append_stroke (self, row, data->rule->strokes.count - 1);
			break;
		case LIEXT_BRUSHES_ROWTYPE_STROKE:
			row = liwdg_treerow_get_parent (row);
			data = liwdg_treerow_get_data (row);
			assert (data != NULL);
			assert (data->type == LIEXT_BRUSHES_ROWTYPE_RULE);
			if (!ligen_rule_insert_stroke (data->rule, 0, 0, 0, 0, 0))
				return 0;
			private_append_stroke (self, row, data->rule->strokes.count - 1);
			break;
	}

	return 0;
}

static int
private_remove (liextBrushes* self,
                liwdgWidget*  widget)
{
	int c[3];
	int i[3];
	liwdgTreerow* row;
	liwdgTreerow* rows[4];
	liextBrushesTreerow* data[4];

	/* Get active row. */
	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return 0;
	data[0] = liwdg_treerow_get_data (row);
	assert (data[0] != NULL);

	/* Remove context specific data. */
	switch (data[0]->type)
	{
		case LIEXT_BRUSHES_ROWTYPE_ROOT:
			break;
		case LIEXT_BRUSHES_ROWTYPE_BRUSH:
			rows[0] = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
			private_remove_strokes (self, rows[0], data[0]->brush->id);
			ligen_generator_remove_brush (self->generator, data[0]->brush->id);
			liwdg_treerow_remove_row (
				liwdg_treerow_get_parent (row),
				liwdg_treerow_get_index (row));
			free (data[0]);
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECTS:
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECT:
			break;
		case LIEXT_BRUSHES_ROWTYPE_RULES:
			break;
		case LIEXT_BRUSHES_ROWTYPE_RULE:
			ligen_brush_remove_rule (data[0]->brush, data[0]->rule->id);
			liwdg_treerow_remove_row (
				liwdg_treerow_get_parent (row),
				liwdg_treerow_get_index (row));
			free (data[0]);
			break;
		case LIEXT_BRUSHES_ROWTYPE_STROKE:
			ligen_rule_remove_stroke (data[0]->rule, data[0]->stroke);
			rows[0] = liwdg_treerow_get_parent (row);
			data[1] = liwdg_treerow_get_data (rows[0]);
			c[0] = liwdg_treerow_get_row_count (rows[0]);
			for (i[0] = 0 ; i[0] < c[0] ; i[0]++)
			{
				rows[1] = liwdg_treerow_get_row (rows[0], i[0]);
				data[2] = liwdg_treerow_get_data (rows[1]);
				data[2]->stroke = i[0];
			}
			liwdg_treerow_remove_row (
				liwdg_treerow_get_parent (row),
				liwdg_treerow_get_index (row));
			free (data[0]);
			break;
	}

	return 0;
}

static int
private_rename (liextBrushes* self,
                liwdgWidget*  widget)
{
	const char* name;
	lialgU32dicIter iter;
	liextBrushesTreerow* row;
	ligenBrush* brush;
	liwdgTreerow* trow;

	row = private_get_active (self);
	if (row == NULL || row->brush == NULL)
		return 0;

	/* Brush? */
	if (row->rule == NULL)
	{
		name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
		trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
		ligen_brush_set_name (row->brush, name);
		liwdg_treerow_set_text (trow, name);
		trow = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
		private_rename_strokes (self, trow, row->brush->id, name);
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

static liwdgTreerow*
private_append_brush (liextBrushes* self,
                      ligenBrush*   brush)
{
	int i;
	ligenRule* rule;
	liwdgTreerow* rows[5] = { NULL, NULL, NULL, NULL, NULL };
	liextBrushesTreerow* data[5] = { NULL, NULL, NULL, NULL, NULL };

	/* Get brushes row. */
	rows[0] = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
	rows[0] = liwdg_treerow_get_row (rows[0], 0);
	data[0] = liwdg_treerow_get_data (rows[0]);
	assert (data[0] != NULL);
	assert (data[0]->type == LIEXT_BRUSHES_ROWTYPE_ROOT);
	assert (data[0]->brush == NULL);
	assert (data[0]->rule == NULL);
	assert (data[0]->stroke < 0);

	/* Add brush row. */
	data[1] = calloc (1, sizeof (liextBrushesTreerow));
	if (data[1] == NULL)
		goto error;
	data[1]->type = LIEXT_BRUSHES_ROWTYPE_BRUSH;
	data[1]->brush = brush;
	data[1]->rule = NULL;
	data[1]->stroke = -1;
	rows[1] = liwdg_treerow_append_row (rows[0], brush->name, data[1]);
	if (rows[1] == NULL)
		goto error;

	/* Add objects row. */
	data[2] = calloc (1, sizeof (liextBrushesTreerow));
	if (data[2] == NULL)
		goto error;
	data[2]->type = LIEXT_BRUSHES_ROWTYPE_OBJECTS;
	data[2]->brush = brush;
	data[2]->rule = NULL;
	data[2]->stroke = -1;
	rows[2] = liwdg_treerow_append_row (rows[1], "Objects", data[2]);
	if (rows[2] == NULL)
		goto error;

	/* Add rules row. */
	data[3] = calloc (1, sizeof (liextBrushesTreerow));
	if (data[3] == NULL)
		goto error;
	data[3]->type = LIEXT_BRUSHES_ROWTYPE_RULES;
	data[3]->brush = brush;
	data[3]->rule = NULL;
	data[3]->stroke = -1;
	rows[3] = liwdg_treerow_append_row (rows[1], "Rules", data[3]);
	if (rows[3] == NULL)
		goto error;

	/* Append rules. */
	for (i = 0 ; i < brush->rules.count ; i++)
	{
		rule = brush->rules.array[i];
		rows[4] = private_append_rule (self, rows[3], rule);
		if (rows[4] == NULL)
		{
			while (liwdg_treerow_get_row_count (rows[3]))
			{
				rows[4] = liwdg_treerow_get_row (rows[3], 0);
				data[4] = liwdg_treerow_get_data (rows[4]);
				free (data[4]);
				liwdg_treerow_remove_row (rows[3], 0);
			}
			goto error;
		}
	}

	return rows[1];

error:
	free (data[0]);
	free (data[1]);
	free (data[2]);
	if (rows[1] != NULL)
		liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (rows[1]));
	return NULL;
}

static liwdgTreerow*
private_append_rule (liextBrushes* self,
                     liwdgTreerow* row,
                     ligenRule*    rule)
{
	int i;
	liextBrushesTreerow* data[2];
	liwdgTreerow* rows[3];

	/* Get row info. */
	data[0] = liwdg_treerow_get_data (row);
	assert (data[0] != NULL);
	assert (data[0]->type == LIEXT_BRUSHES_ROWTYPE_RULES);
	assert (data[0]->brush != NULL);
	assert (data[0]->rule == NULL);
	assert (data[0]->stroke < 0);
	rows[0] = row;

	/* Add rule info. */
	data[1] = calloc (1, sizeof (liextBrushesTreerow));
	data[1]->type = LIEXT_BRUSHES_ROWTYPE_RULE;
	data[1]->brush = data[0]->brush;
	data[1]->rule = rule;
	data[1]->stroke = -1;
	rows[1] = liwdg_treerow_append_row (rows[0], rule->name, data[1]);
	if (rows[1] == NULL)
	{
		free (data[1]);
		return NULL;
	}

	/* Append strokes. */
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		rows[2] = private_append_stroke (self, rows[1], i);
		if (rows[2] == NULL)
		{
			while (liwdg_treerow_get_row_count (rows[1]))
			{
				rows[2] = liwdg_treerow_get_row (rows[1], 0);
				data[2] = liwdg_treerow_get_data (rows[2]);
				free (data[2]);
				liwdg_treerow_remove_row (rows[1], 0);
			}
			liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (rows[1]));
			free (data[1]);
			return NULL;
		}
	}

	return rows[1];
}

static liwdgTreerow*
private_append_stroke (liextBrushes* self,
                       liwdgTreerow* row,
                       int           index)
{
	liextBrushesTreerow* data[2];
	ligenBrush* brush;
	ligenRulestroke* stroke;
	liwdgTreerow* rows[2];

	/* Get row info. */
	data[0] = liwdg_treerow_get_data (row);
	assert (data[0] != NULL);
	assert (data[0]->type == LIEXT_BRUSHES_ROWTYPE_RULE);
	assert (data[0]->brush != NULL);
	assert (data[0]->rule != NULL);
	assert (data[0]->stroke < 0);
	rows[0] = row;

	/* Get stroke info. */
	assert (index >= 0);
	assert (index < data[0]->rule->strokes.count);
	stroke = data[0]->rule->strokes.array + index;
	brush = ligen_generator_find_brush (self->generator, stroke->brush);
	assert (brush != NULL);

	/* Add stroke row. */
	data[1] = calloc (1, sizeof (liextBrushesTreerow));
	data[1]->type = LIEXT_BRUSHES_ROWTYPE_STROKE;
	data[1]->brush = data[0]->brush;
	data[1]->rule = data[0]->rule;
	data[1]->stroke = index;
	rows[1] = liwdg_treerow_append_row (rows[0], brush->name, data[1]);
	if (rows[1] == NULL)
	{
		free (data[1]);
		return NULL;
	}

	return rows[1];
}

static void
private_remove_strokes (liextBrushes* self,
                        liwdgTreerow* row,
                        int           brush)
{
	int c;
	int i;
	liextBrushesTreerow* data;
	liwdgTreerow* row1;

	c = liwdg_treerow_get_row_count (row);
	for (i = 0 ; i < c ; i++)
	{
		row1 = liwdg_treerow_get_row (row, i);
		data = liwdg_treerow_get_data (row1);
		if (data->type == LIEXT_BRUSHES_ROWTYPE_STROKE &&
		    data->rule->strokes.array[data->stroke].brush == brush)
		{
			liwdg_treerow_remove_row (row, i);
			free (data);
			i--;
			c--;
		}
		else
			private_remove_strokes (self, row1, brush);
	}
}

static void
private_rename_strokes (liextBrushes* self,
                        liwdgTreerow* row,
                        int           brush,
                        const char*   text)
{
	int c;
	int i;
	liextBrushesTreerow* data;
	liwdgTreerow* row1;

	c = liwdg_treerow_get_row_count (row);
	for (i = 0 ; i < c ; i++)
	{
		row1 = liwdg_treerow_get_row (row, i);
		data = liwdg_treerow_get_data (row1);
		if (data->type == LIEXT_BRUSHES_ROWTYPE_STROKE &&
		    data->rule->strokes.array[data->stroke].brush == brush)
			liwdg_treerow_set_text (row1, text);
		else
			private_rename_strokes (self, row1, brush, text);
	}
}

static void
private_populate (liextBrushes* self)
{
	lialgU32dicIter iter;
	liextBrushesTreerow* data;
	ligenBrush* brush;
	liwdgTreerow* row;

	/* Clear tree. */
	row = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
	liwdg_tree_foreach (LIWDG_TREE (self->widgets.tree), free);
	liwdg_tree_clear (LIWDG_TREE (self->widgets.tree));

	/* Add root row. */
	data = calloc (1, sizeof (liextBrushesTreerow));
	data->type = LIEXT_BRUSHES_ROWTYPE_ROOT;
	data->brush = NULL;
	data->rule = NULL;
	data->stroke = -1;
	if (!liwdg_treerow_append_row (row, "Brushes", data))
	{
		free (data);
		return;
	}

	/* Append brushes. */
	LI_FOREACH_U32DIC (iter, self->generator->brushes)
	{
		brush = iter.value;
		private_append_brush (self, brush);
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
