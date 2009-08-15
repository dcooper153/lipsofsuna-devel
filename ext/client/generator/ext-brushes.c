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
	int object;
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

static liwdgTreerow*
private_append_brush (liextBrushes* self,
                      ligenBrush*   brush);

static liwdgTreerow*
private_append_object (liextBrushes* self,
                       liwdgTreerow* row,
                       int           index);

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
private_transform (liextBrushes*   self,
                   limatTransform* transform,
                   int             finish);

static void
private_resize_brush (liextBrushes* self);

static void
private_rebuild_brush (liextBrushes* self);

static void
private_rebuild_preview (liextBrushes* self);

static void
private_rebuild_selection (liextBrushes* self);

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
	liext_preview_set_transform_call (LIEXT_PREVIEW (data->widgets.preview), private_transform, self);
	liwdg_widget_set_request (data->widgets.preview, 320, 240);
	liwdg_group_set_child (LIWDG_GROUP (data->widgets.group_view), 0, 1, data->widgets.preview);
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
	liwdgWidget* group_attr;
	liwdgWidget* group_tree;
	liwdgWidget* widgets[] =
	{
		liwdg_group_new_with_size (manager, 2, 2),
		liwdg_group_new_with_size (manager, 1, 3),
		liwdg_group_new_with_size (manager, 3, 1),
		liwdg_group_new_with_size (manager, 2, 4),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_entry_new (manager),
		liwdg_label_new (manager),
		liwdg_label_new (manager),
		liwdg_spin_new (manager),
		liwdg_spin_new (manager),
		liwdg_spin_new (manager),
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
	group_attr = widgets[(i = 0)];
	group_tree = widgets[++i];
	self->widgets.group_size = widgets[++i];
	self->widgets.group_view = widgets[++i];
	self->widgets.button_add = widgets[++i];
	self->widgets.button_remove = widgets[++i];
	self->widgets.entry_name = widgets[++i];
	self->widgets.label_size = widgets[++i];
	self->widgets.label_type = widgets[++i];
	self->widgets.spin_sizex = widgets[++i];
	self->widgets.spin_sizey = widgets[++i];
	self->widgets.spin_sizez = widgets[++i];
	self->widgets.tree = widgets[++i];

	/* Configure widgets. */
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_add), "Add");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_remove), "Remove");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_size), "Size");
	liwdg_widget_insert_callback (self->widgets.button_add, LIWDG_CALLBACK_PRESSED, 0, private_add, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_remove, LIWDG_CALLBACK_PRESSED, 0, private_remove, self, NULL);
	liwdg_widget_insert_callback (self->widgets.entry_name, LIWDG_CALLBACK_EDITED, 0, private_rename, self, NULL);
	liwdg_widget_insert_callback (self->widgets.tree, LIWDG_CALLBACK_PRESSED, 0, private_selected, self, NULL);
	liwdg_widget_insert_callback (self->widgets.spin_sizex, LIWDG_CALLBACK_PRESSED, 0, private_resize_brush, self, NULL);
	liwdg_widget_insert_callback (self->widgets.spin_sizey, LIWDG_CALLBACK_PRESSED, 0, private_resize_brush, self, NULL);
	liwdg_widget_insert_callback (self->widgets.spin_sizez, LIWDG_CALLBACK_PRESSED, 0, private_resize_brush, self, NULL);

	/* Tree. */
	liwdg_group_set_row_expand (LIWDG_GROUP (group_tree), 2, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_tree), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 2, self->widgets.tree);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 1, self->widgets.button_add);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 0, self->widgets.button_remove);

	/* View. */
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_size), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self->widgets.group_size), 0, 1);
	liwdg_group_set_homogeneous (LIWDG_GROUP (self->widgets.group_size), 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_size), 0, 0, self->widgets.spin_sizex);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_size), 1, 0, self->widgets.spin_sizey);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_size), 2, 0, self->widgets.spin_sizez);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_attr), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 0, 1, self->widgets.label_type);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 1, 1, self->widgets.entry_name);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 0, 0, self->widgets.label_size);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 1, 0, self->widgets.group_size);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_view), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self->widgets.group_view), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_view), 0, 0, group_attr);
	liwdg_widget_set_visible (self->widgets.label_size, 0);
	liwdg_widget_set_visible (self->widgets.group_size, 0);

	/* Pack self. */
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 1, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, group_tree);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 0, self->widgets.group_view);

	self->transform = limat_transform_identity ();

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
	liextBrushesTreerow* data;
	ligenBrush* brush;
	ligenRule* rule;
	limatTransform transform;
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
			brush = ligen_brush_new (self->generator, 10, 10, 10);
			if (brush == NULL)
				return 0;
			if (!ligen_generator_insert_brush (self->generator, brush))
			{
				ligen_brush_free (brush);
				return 0;
			}
			if (!private_append_brush (self, brush))
			{
				ligen_generator_remove_brush (self->generator, brush->id);
				return 0;
			}
			private_rebuild_selection (self);
			break;
		case LIEXT_BRUSHES_ROWTYPE_BRUSH:
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECT:
			row = liwdg_treerow_get_parent (row);
			data = liwdg_treerow_get_data (row);
			assert (data != NULL);
			assert (data->type == LIEXT_BRUSHES_ROWTYPE_OBJECTS);
			/* Fall through. */
		case LIEXT_BRUSHES_ROWTYPE_OBJECTS:
			transform = limat_transform_init (
				limat_vector_init (0.0f, 0.0f, 0.0f),
				limat_quaternion_identity ());
			if (!ligen_brush_insert_object (data->brush, 0, 1.0f, "", "", "", &transform))
				return 0;
			if (!private_append_object (self, row, data->brush->objects.count - 1))
			{
				ligen_brush_remove_object (data->brush, data->brush->objects.count - 1);
				return 0;
			}
			private_rebuild_selection (self);
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
			private_rebuild_selection (self);
			break;
		case LIEXT_BRUSHES_ROWTYPE_STROKE:
			row = liwdg_treerow_get_parent (row);
			data = liwdg_treerow_get_data (row);
			assert (data != NULL);
			assert (data->type == LIEXT_BRUSHES_ROWTYPE_RULE);
			/* Fall through. */
		case LIEXT_BRUSHES_ROWTYPE_RULE:
			if (!ligen_rule_insert_stroke (data->rule, 0, 0, 0, 0, 0))
				return 0;
			private_append_stroke (self, row, data->rule->strokes.count - 1);
			private_rebuild_selection (self);
			break;
	}

	return 0;
}

static int
private_remove (liextBrushes* self,
                liwdgWidget*  widget)
{
	int c;
	int i;
	liwdgTreerow* row;
	liwdgTreerow* rows[2];
	liextBrushesTreerow* data[3];

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
			rows[0] = liwdg_treerow_get_parent (row);
			liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (row));
			ligen_generator_remove_brush (self->generator, data[0]->brush->id);
			free (data[0]);
			private_rebuild_selection (self);
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECTS:
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECT:
			rows[0] = liwdg_treerow_get_parent (row);
			liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (row));
			ligen_brush_remove_object (data[0]->brush, data[0]->object);
			free (data[0]);
			c = liwdg_treerow_get_row_count (rows[0]);
			for (i = 0 ; i < c ; i++)
			{
				rows[1] = liwdg_treerow_get_row (rows[0], i);
				data[1] = liwdg_treerow_get_data (rows[1]);
				data[1]->object = i;
			}
			private_rebuild_selection (self);
			break;
		case LIEXT_BRUSHES_ROWTYPE_RULES:
			break;
		case LIEXT_BRUSHES_ROWTYPE_RULE:
			rows[0] = liwdg_treerow_get_parent (row);
			ligen_brush_remove_rule (data[0]->brush, data[0]->rule->id);
			liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (row));
			free (data[0]);
			private_rebuild_selection (self);
			break;
		case LIEXT_BRUSHES_ROWTYPE_STROKE:
			rows[0] = liwdg_treerow_get_parent (row);
			liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (row));
			ligen_rule_remove_stroke (data[0]->rule, data[0]->stroke);
			free (data[0]);
			c = liwdg_treerow_get_row_count (rows[0]);
			for (i = 0 ; i < c ; i++)
			{
				rows[1] = liwdg_treerow_get_row (rows[0], i);
				data[1] = liwdg_treerow_get_data (rows[1]);
				data[1]->stroke = i;
			}
			private_rebuild_selection (self);
			break;
	}

	return 0;
}

static int
private_rename (liextBrushes* self,
                liwdgWidget*  widget)
{
	char* tmp;
	const char* name;
	lialgU32dicIter iter;
	ligenBrush* brush;
	ligenBrushobject* object;
	liextBrushesTreerow* data;
	liwdgTreerow* row;

	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return 0;
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);

	switch (data->type)
	{
		case LIEXT_BRUSHES_ROWTYPE_ROOT:
			break;
		case LIEXT_BRUSHES_ROWTYPE_BRUSH:
		case LIEXT_BRUSHES_ROWTYPE_OBJECTS:
		case LIEXT_BRUSHES_ROWTYPE_RULES:
			name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
			ligen_brush_set_name (data->brush, name);
			liwdg_treerow_set_text (row, name);
			row = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
			private_rename_strokes (self, row, data->brush->id, name);
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECT:
			name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
			tmp = strdup (name);
			if (tmp == NULL)
				return 0;
			object = data->brush->objects.array[data->object];
			free (object->model);
			object->model = tmp;
			liwdg_treerow_set_text (row, name);
			private_rebuild_preview (self);
			break;
		case LIEXT_BRUSHES_ROWTYPE_RULE:
			name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
			ligen_rule_set_name (data->rule, name);
			liwdg_treerow_set_text (row, name);
			break;
		case LIEXT_BRUSHES_ROWTYPE_STROKE:
			name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
			LI_FOREACH_U32DIC (iter, self->generator->brushes)
			{
				brush = iter.value;
				if (!strcmp (name, data->brush->name))
				{
					data->rule->strokes.array[data->stroke].brush = brush->id;
					liwdg_treerow_set_text (row, name);
					private_rebuild_preview (self);
					break;
				}
			}
			break;
	}

	return 0;
}

static int
private_selected (liextBrushes* self,
                  liwdgWidget*  widget,
                  liwdgTreerow* row)
{
	liextBrushesTreerow* data;
	liwdgTreerow* row0;

	/* Deselect old. */
	row0 = liwdg_tree_get_active (LIWDG_TREE (widget));
	if (row0 != NULL)
		liwdg_treerow_set_highlighted (row0, 0);

	/* Select new. */
	liwdg_treerow_set_highlighted (row, 1);
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);

	/* Update info. */
	private_rebuild_selection (self);

	return 0;
}

static liwdgTreerow*
private_append_brush (liextBrushes* self,
                      ligenBrush*   brush)
{
	int i;
	ligenBrushobject* object;
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
	assert (data[0]->object < 0);
	assert (data[0]->stroke < 0);

	/* Add brush row. */
	data[1] = calloc (1, sizeof (liextBrushesTreerow));
	if (data[1] == NULL)
		goto error;
	data[1]->type = LIEXT_BRUSHES_ROWTYPE_BRUSH;
	data[1]->brush = brush;
	data[1]->rule = NULL;
	data[1]->object = -1;
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
	data[2]->object = -1;
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
	data[3]->object = -1;
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

	/* Append objects. */
	for (i = 0 ; i < brush->objects.count ; i++)
	{
		object = brush->objects.array[i];
		rows[4] = private_append_object (self, rows[2], i);
		if (rows[4] == NULL)
		{
			while (liwdg_treerow_get_row_count (rows[2]))
			{
				rows[4] = liwdg_treerow_get_row (rows[2], 0);
				data[4] = liwdg_treerow_get_data (rows[4]);
				free (data[4]);
				liwdg_treerow_remove_row (rows[2], 0);
			}
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

	/* Set active. */
	rows[0] = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (rows[0] != NULL)
		liwdg_treerow_set_highlighted (rows[0], 0);
	liwdg_treerow_set_highlighted (rows[1], 1);
	rows[0] = liwdg_treerow_get_parent (rows[1]);
	liwdg_treerow_set_expanded (rows[0], 1);

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
private_append_object (liextBrushes* self,
                       liwdgTreerow* row,
                       int           index)
{
	ligenBrushobject* object;
	liextBrushesTreerow* data[2];
	liwdgTreerow* rows[2];

	/* Get row info. */
	data[0] = liwdg_treerow_get_data (row);
	assert (data[0] != NULL);
	assert (data[0]->type == LIEXT_BRUSHES_ROWTYPE_OBJECTS);
	assert (data[0]->brush != NULL);
	assert (data[0]->rule == NULL);
	assert (data[0]->object < 0);
	assert (data[0]->stroke < 0);
	rows[0] = row;

	/* Get object info. */
	assert (index >= 0);
	assert (index < data[0]->brush->objects.count);
	object = data[0]->brush->objects.array[index];

	/* Add object row. */
	data[1] = calloc (1, sizeof (liextBrushesTreerow));
	data[1]->type = LIEXT_BRUSHES_ROWTYPE_OBJECT;
	data[1]->brush = data[0]->brush;
	data[1]->rule = NULL;
	data[1]->object = index;
	data[1]->stroke = -1;
	rows[1] = liwdg_treerow_append_row (rows[0], object->model, data[1]);
	if (rows[1] == NULL)
	{
		free (data[1]);
		return NULL;
	}

	/* Set active. */
	rows[0] = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (rows[0] != NULL)
		liwdg_treerow_set_highlighted (rows[0], 0);
	liwdg_treerow_set_highlighted (rows[1], 1);
	rows[0] = liwdg_treerow_get_parent (rows[1]);
	liwdg_treerow_set_expanded (rows[0], 1);

	return rows[1];
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
	assert (data[0]->object < 0);
	assert (data[0]->stroke < 0);
	rows[0] = row;

	/* Add rule row. */
	data[1] = calloc (1, sizeof (liextBrushesTreerow));
	data[1]->type = LIEXT_BRUSHES_ROWTYPE_RULE;
	data[1]->brush = data[0]->brush;
	data[1]->rule = rule;
	data[1]->object = -1;
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

	/* Set active. */
	rows[0] = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (rows[0] != NULL)
		liwdg_treerow_set_highlighted (rows[0], 0);
	liwdg_treerow_set_highlighted (rows[1], 1);
	rows[0] = liwdg_treerow_get_parent (rows[1]);
	liwdg_treerow_set_expanded (rows[0], 1);

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
	assert (data[0]->object < 0);
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
	data[1]->object = -1;
	data[1]->stroke = index;
	rows[1] = liwdg_treerow_append_row (rows[0], brush->name, data[1]);
	if (rows[1] == NULL)
	{
		free (data[1]);
		return NULL;
	}

	/* Set active. */
	rows[0] = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (rows[0] != NULL)
		liwdg_treerow_set_highlighted (rows[0], 0);
	liwdg_treerow_set_highlighted (rows[1], 1);
	rows[0] = liwdg_treerow_get_parent (rows[1]);
	liwdg_treerow_set_expanded (rows[0], 1);

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
private_transform (liextBrushes*   self,
                   limatTransform* transform,
                   int             finish)
{
	ligenBrushobject* object;
	ligenRulestroke* stroke;
	liextBrushesTreerow* row;
	limatQuaternion quat;
	limatVector pos;

	row = private_get_active (self);
	if (finish)
	{
		if (row != NULL && row->stroke >= 0)
		{
			stroke = row->rule->strokes.array + row->stroke;
			stroke->pos[0] += (int)(round (transform->position.x / LIVOX_TILE_WIDTH));
			stroke->pos[1] += (int)(round (transform->position.y / LIVOX_TILE_WIDTH));
			stroke->pos[2] += (int)(round (transform->position.z / LIVOX_TILE_WIDTH));
		}
		else if (row != NULL && row->object >= 0)
		{
			object = row->brush->objects.array[row->object];
			pos = limat_vector_add (transform->position, object->transform.position);
			quat = limat_quaternion_multiply (transform->rotation, object->transform.rotation);
			object->transform = limat_transform_init (pos, quat);
		}
		else if (row->type == LIEXT_BRUSHES_ROWTYPE_BRUSH)
		{
			self->transform = limat_transform_multiply (*transform, self->transform);
		}
	}
	else if (row->type == LIEXT_BRUSHES_ROWTYPE_BRUSH)
	{
		private_rebuild_brush (self);
		return;
	}
	private_rebuild_preview (self);
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
	data->object = -1;
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
private_resize_brush (liextBrushes* self)
{
	int x;
	int y;
	int z;
	liextBrushesTreerow* data;
	liwdgTreerow* row;

	/* Find the brush. */
	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	assert (row != NULL);
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);
	assert (data->brush != NULL); 

	/* Resize the brush. */
	x = (int) liwdg_spin_get_value (LIWDG_SPIN (self->widgets.spin_sizex));
	y = (int) liwdg_spin_get_value (LIWDG_SPIN (self->widgets.spin_sizey));
	z = (int) liwdg_spin_get_value (LIWDG_SPIN (self->widgets.spin_sizez));
	ligen_brush_set_size (data->brush, x, y, z);

	/* Rebuild preview. */
	private_rebuild_preview (self);
}

static void
private_rebuild_brush (liextBrushes* self)
{
	int x;
	int y;
	int z;
	liengObject* player;
	liextBrushesTreerow* data;
	ligenBrush* brush;
	limatTransform transform;
	limatTransform transform0;
	limatTransform transform1;
	liwdgTreerow* row;

	/* Find the brush. */
	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return;
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);
	assert (data->brush != NULL); 
	brush = data->brush;

	/* Find the player. */
	if (self->module->module->network == NULL)
		return;
	player = licli_module_get_player (self->module->module);
	if (player == NULL)
		return;

	/* Copy voxels from the scene. */
	lieng_object_get_transform (player, &transform0);
	liext_preview_get_transform (LIEXT_PREVIEW (self->widgets.preview), &transform1);
	transform = limat_transform_multiply (transform1, transform0);
	transform = limat_transform_multiply (transform, self->transform);
	x = (int)(round (transform.position.x / LIVOX_TILE_WIDTH));
	y = (int)(round (transform.position.y / LIVOX_TILE_WIDTH));
	z = (int)(round (transform.position.z / LIVOX_TILE_WIDTH));
	livox_manager_copy_voxels (self->module->module->voxels, x, y, z,
		brush->size[0], brush->size[1], brush->size[2], brush->voxels.array);

	/* Rebuild preview. */
	private_rebuild_preview (self);
}

static void
private_rebuild_preview (liextBrushes* self)
{
	int i;
	int j;
	int pos[3];
	liextBrushesTreerow* row;
	ligenBrush* brush;
	ligenBrushobject* object;
	ligenRulestroke* stroke;
	limatQuaternion quat;
	limatTransform transform;
	limatVector vec;

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
		for (j = 0 ; j < row->brush->objects.count ; j++)
		{
			object = row->brush->objects.array[j];
			if (row->object == j)
			{
				liext_preview_get_transform (LIEXT_PREVIEW (self->widgets.preview), &transform);
				vec = limat_vector_add (transform.position, object->transform.position);
				quat = limat_quaternion_multiply (transform.rotation, object->transform.rotation);
				transform = limat_transform_init (vec, quat);
			}
			else
				transform = object->transform;
			liext_preview_insert_object (LIEXT_PREVIEW (self->widgets.preview), &transform, object->model);
		}
		liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
	}

	/* Rule? */
	else
	{
		liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
		for (i = 0 ; i < row->rule->strokes.count ; i++)
		{
			stroke = row->rule->strokes.array + i;
			pos[0] = stroke->pos[0];
			pos[1] = stroke->pos[1];
			pos[2] = stroke->pos[2];
			if (row->stroke == i)
			{
				liext_preview_get_transform (LIEXT_PREVIEW (self->widgets.preview), &transform);
				pos[0] += (int)(round (transform.position.x / LIVOX_TILE_WIDTH));
				pos[1] += (int)(round (transform.position.y / LIVOX_TILE_WIDTH));
				pos[2] += (int)(round (transform.position.z / LIVOX_TILE_WIDTH));
			}
			liext_preview_insert_stroke (LIEXT_PREVIEW (self->widgets.preview),
				pos[0], pos[1], pos[2], stroke->brush);
		}
		liext_preview_insert_stroke (LIEXT_PREVIEW (self->widgets.preview), 0, 0, 0, row->brush->id);
		for (i = 0 ; i < row->rule->strokes.count ; i++)
		{
			stroke = row->rule->strokes.array + i;
			brush = ligen_generator_find_brush (self->generator, stroke->brush);
			if (brush == NULL)
				continue;
			for (j = 0 ; j < brush->objects.count ; j++)
			{
				object = brush->objects.array[j];
				transform = limat_convert_vector_to_transform (limat_vector_init (
					stroke->pos[0], stroke->pos[1], stroke->pos[2]));
				transform = limat_transform_multiply (object->transform, transform);
				liext_preview_insert_object (LIEXT_PREVIEW (self->widgets.preview), &transform, object->model);
			}
		}
		for (j = 0 ; j < row->brush->objects.count ; j++)
		{
			object = row->brush->objects.array[j];
			transform = object->transform;
			liext_preview_insert_object (LIEXT_PREVIEW (self->widgets.preview), &transform, object->model);
		}
		liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
	}
}

static void
private_rebuild_selection (liextBrushes* self)
{
	liextBrushesTreerow* data;
	ligenBrush* brush;
	ligenBrushobject* object;
	ligenRulestroke* stroke;
	liwdgTreerow* row;

	/* Get active row. */
	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), "");
		private_rebuild_preview (self);
		return;
	}
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);

	/* Set info. */
	if (data->brush == NULL)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), "");
		liwdg_widget_set_visible (self->widgets.label_size, 0);
		liwdg_widget_set_visible (self->widgets.group_size, 0);
	}
	else if (data->object >= 0)
	{
		object = data->brush->objects.array[data->object];
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Object:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), object->model);
		liwdg_widget_set_visible (self->widgets.label_size, 0);
		liwdg_widget_set_visible (self->widgets.group_size, 0);
	}
	else if (data->rule == NULL)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Brush:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), data->brush->name);
		liwdg_widget_set_visible (self->widgets.label_size, 1);
		liwdg_widget_set_visible (self->widgets.group_size, 1);
		liwdg_spin_set_value (LIWDG_SPIN (self->widgets.spin_sizex), data->brush->size[0]);
		liwdg_spin_set_value (LIWDG_SPIN (self->widgets.spin_sizey), data->brush->size[1]);
		liwdg_spin_set_value (LIWDG_SPIN (self->widgets.spin_sizez), data->brush->size[2]);
	}
	else if (data->stroke < 0)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Rule:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), data->rule->name);
		liwdg_widget_set_visible (self->widgets.label_size, 0);
		liwdg_widget_set_visible (self->widgets.group_size, 0);
	}
	else
	{
		stroke = data->rule->strokes.array + data->stroke;
		brush = ligen_generator_find_brush (self->generator, stroke->brush);
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Stroke:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), brush != NULL? brush->name : "");
		liwdg_widget_set_visible (self->widgets.label_size, 0);
		liwdg_widget_set_visible (self->widgets.group_size, 0);
	}

	/* Rebuild preview. */
	private_rebuild_preview (self);
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
