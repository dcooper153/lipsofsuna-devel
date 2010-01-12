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
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#include <lipsofsuna/system.h>
#include "ext-brushes.h"
#include "ext-dialog.h"
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

typedef struct _LIExtBrushesTreerow LIExtBrushesTreerow;
struct _LIExtBrushesTreerow
{
	LIGenBrush* brush;
	LIGenRule* rule;
	int stroke;
	int object;
	int type;
};

static const void*
private_base ();

static int
private_init (LIExtBrushes* self,
              LIWdgManager* manager);

static void
private_free (LIExtBrushes* self);

static int
private_event (LIExtBrushes* self,
               liwdgEvent*   event);

static int
private_add (LIExtBrushes* self,
             LIWdgWidget*  widget);

static int
private_copy (LIExtBrushes* self,
              LIWdgWidget*  widget);

static int
private_object_info (LIExtBrushes* self);

static int
private_paint_select (LIExtBrushes* self,
                      LIWdgWidget*  widget);

static int
private_paint_terrain (LIExtBrushes* self,
                       int           x,
                       int           y);

static int
private_remove (LIExtBrushes* self,
                LIWdgWidget*  widget);

static int
private_rename (LIExtBrushes* self,
                LIWdgWidget*  widget);

static int
private_selected (LIExtBrushes* self,
                  LIWdgWidget*  widget,
                  LIWdgTreerow* row);

static int
private_stroke_flags (LIExtBrushes* self);

static LIWdgTreerow*
private_append_brush (LIExtBrushes* self,
                      LIGenBrush*   brush);

static LIWdgTreerow*
private_append_object (LIExtBrushes* self,
                       LIWdgTreerow* row,
                       int           index);

static LIWdgTreerow*
private_append_rule (LIExtBrushes* self,
                     LIWdgTreerow* row,
                     LIGenRule*    rule);

static LIWdgTreerow*
private_append_stroke (LIExtBrushes* self,
                       LIWdgTreerow* row,
                       int           index);

static void
private_remove_strokes (LIExtBrushes* self,
                        LIWdgTreerow* row,
                        int           brush);

static void
private_rename_strokes (LIExtBrushes* self,
                        LIWdgTreerow* row,
                        int           brush,
                        const char*   text);

static void
private_populate (LIExtBrushes* self);

static void
private_transform (LIExtBrushes*   self,
                   LIMatTransform* transform,
                   int             finish);

static void
private_resize_brush (LIExtBrushes* self);

static void
private_rebuild_brush (LIExtBrushes* self);

static void
private_rebuild_preview (LIExtBrushes* self);

static void
private_rebuild_selection (LIExtBrushes* self);

static LIExtBrushesTreerow*
private_get_active (LIExtBrushes* self);

/****************************************************************************/

const LIWdgClass liext_widget_brushes =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorBrushes", sizeof (LIExtBrushes),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event
};

LIWdgWidget*
liext_brushes_new (LIWdgManager* manager,
                   LIExtModule*  module)
{
	LIExtBrushes* data;
	LIWdgWidget* self;

	self = liwdg_widget_new (manager, &liext_widget_brushes);
	if (self == NULL)
		return NULL;
	data = LIEXT_BRUSHES (self);
	data->module = module;

	/* Initialize scene preview. */
	data->widgets.preview = liext_preview_new (manager, module->client);
	if (data->widgets.preview == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liwdg_widget_insert_callback (data->widgets.preview, "pressed", private_paint_terrain, self);
	liwdg_widget_insert_callback (data->widgets.preview, "transform", private_transform, self);
	liwdg_widget_set_request (data->widgets.preview, 440, 240);
	liwdg_group_set_child (LIWDG_GROUP (data->widgets.group_view), 0, 1, data->widgets.preview);
	data->generator = LIEXT_PREVIEW (data->widgets.preview)->generator;

	/* Populate brush list. */
	private_populate (data);

	return self;
}

int
liext_brushes_insert_brush (LIExtBrushes* self,
                            LIGenBrush*   brush)
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
liext_brushes_save (LIExtBrushes* self)
{
	return ligen_generator_write_brushes (self->generator);
}

void
liext_brushes_reset (LIExtBrushes* self,
                     LIArcReader*  reader)
{
	reader->pos = 1;
	liext_preview_replace_materials (LIEXT_PREVIEW (self->widgets.preview), reader);
	private_populate (self);
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdg_widget_group;
}

static int
private_init (LIExtBrushes* self,
              LIWdgManager* manager)
{
	LIWdgWidget* group_attr;
	LIWdgWidget* group_tree;

	/* Allocate widgets. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 3, 1))
		return 0;
	if (!liwdg_manager_alloc_widgets (manager,
		&self->widgets.check_required, liwdg_check_new (manager),
		&group_attr, liwdg_group_new_with_size (manager, 2, 6),
		&group_tree, liwdg_group_new_with_size (manager, 1, 4),
		&self->widgets.group_paint, liwdg_group_new_with_size (manager, 7, 1),
		&self->widgets.group_size, liwdg_group_new_with_size (manager, 3, 1),
		&self->widgets.group_view, liwdg_group_new_with_size (manager, 1, 3),
		&self->widgets.button_add, liwdg_button_new (manager),
		&self->widgets.button_copy, liwdg_button_new (manager),
		&self->widgets.button_paint[0], liwdg_button_new (manager),
		&self->widgets.button_paint[1], liwdg_button_new (manager),
		&self->widgets.button_paint[2], liwdg_button_new (manager),
		&self->widgets.button_paint[3], liwdg_button_new (manager),
		&self->widgets.button_paint[4], liwdg_button_new (manager),
		&self->widgets.button_paint[5], liwdg_button_new (manager),
		&self->widgets.button_remove, liwdg_button_new (manager),
		&self->widgets.entry_name, liwdg_entry_new (manager),
		&self->widgets.entry_objtype, liwdg_entry_new (manager),
		&self->widgets.entry_objextra, liwdg_entry_new (manager),
		&self->widgets.label_objextra, liwdg_label_new (manager),
		&self->widgets.label_objprob, liwdg_label_new (manager),
		&self->widgets.label_objtype, liwdg_label_new (manager),
		&self->widgets.label_size, liwdg_label_new (manager),
		&self->widgets.label_type, liwdg_label_new (manager),
		&self->widgets.scroll_objprob, liwdg_scroll_new (manager),
		&self->widgets.spin_axis, liwdg_spin_new (manager),
		&self->widgets.spin_sizex, liwdg_spin_new (manager),
		&self->widgets.spin_sizey, liwdg_spin_new (manager),
		&self->widgets.spin_sizez, liwdg_spin_new (manager),
		&self->widgets.tree, liwdg_tree_new (manager),
		&self->widgets.view, liwdg_view_new (manager), NULL))
		return 0;

	/* Configure widgets. */
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_size), "Size:");
	liwdg_widget_insert_callback (self->widgets.entry_name, "edited", private_rename, self);
	liwdg_widget_insert_callback (self->widgets.tree, "pressed", private_selected, self);
	liwdg_widget_insert_callback (self->widgets.spin_sizex, "pressed", private_resize_brush, self);
	liwdg_widget_insert_callback (self->widgets.spin_sizey, "pressed", private_resize_brush, self);
	liwdg_widget_insert_callback (self->widgets.spin_sizez, "pressed", private_resize_brush, self);

	/* Paint. */
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_paint[0]), "Copy");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_paint[1]), "Paste");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_paint[2]), "Erase");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_paint[3]), "Insert");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_paint[4]), "Replace");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_paint[5]), "Rotate");
	liwdg_spin_set_value (LIWDG_SPIN (self->widgets.spin_axis), 1);
	liwdg_widget_insert_callback (self->widgets.button_paint[0], "pressed", private_paint_select, self);
	liwdg_widget_insert_callback (self->widgets.button_paint[1], "pressed", private_paint_select, self);
	liwdg_widget_insert_callback (self->widgets.button_paint[2], "pressed", private_paint_select, self);
	liwdg_widget_insert_callback (self->widgets.button_paint[3], "pressed", private_paint_select, self);
	liwdg_widget_insert_callback (self->widgets.button_paint[4], "pressed", private_paint_select, self);
	liwdg_widget_insert_callback (self->widgets.button_paint[5], "pressed", private_paint_select, self);
	liwdg_group_set_homogeneous (LIWDG_GROUP (self->widgets.group_paint), 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_paint), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_paint), 0, 0, self->widgets.button_paint[0]);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_paint), 1, 0, self->widgets.button_paint[1]);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_paint), 2, 0, self->widgets.button_paint[2]);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_paint), 3, 0, self->widgets.button_paint[3]);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_paint), 4, 0, self->widgets.button_paint[4]);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_paint), 5, 0, self->widgets.button_paint[5]);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_paint), 6, 0, self->widgets.spin_axis);

	/* Tree. */
	liwdg_view_set_vscroll (LIWDG_VIEW (self->widgets.view), 1);
	liwdg_view_set_child (LIWDG_VIEW (self->widgets.view), self->widgets.tree);
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_add), "Add");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_copy), "Copy");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_remove), "Remove");
	liwdg_widget_insert_callback (self->widgets.button_add, "pressed", private_add, self);
	liwdg_widget_insert_callback (self->widgets.button_copy, "pressed", private_copy, self);
	liwdg_widget_insert_callback (self->widgets.button_remove, "pressed", private_remove, self);
	liwdg_group_set_row_expand (LIWDG_GROUP (group_tree), 0, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_tree), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 0, self->widgets.view);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 1, self->widgets.button_add);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 2, self->widgets.button_remove);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 3, self->widgets.button_copy);

	/* View. */
	liwdg_check_set_text (LIWDG_CHECK (self->widgets.check_required), "Required");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_objextra), "Extra:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_objprob), "Probability:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_objtype), "Type:");
	liwdg_widget_insert_callback (self->widgets.check_required, "pressed", private_stroke_flags, self);
	liwdg_widget_insert_callback (self->widgets.entry_objextra, "edited", private_object_info, self);
	liwdg_widget_insert_callback (self->widgets.entry_objtype, "edited", private_object_info, self);
	liwdg_widget_insert_callback (self->widgets.scroll_objprob, "pressed", private_object_info, self);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_size), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self->widgets.group_size), 0, 1);
	liwdg_group_set_homogeneous (LIWDG_GROUP (self->widgets.group_size), 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_size), 0, 0, self->widgets.spin_sizex);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_size), 1, 0, self->widgets.spin_sizey);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_size), 2, 0, self->widgets.spin_sizez);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_attr), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 1, 0, self->widgets.check_required);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 0, 1, self->widgets.label_type);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 1, 1, self->widgets.entry_name);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 0, 2, self->widgets.label_size);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 1, 2, self->widgets.group_size);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 0, 3, self->widgets.label_objtype);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 1, 3, self->widgets.entry_objtype);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 0, 4, self->widgets.label_objextra);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 1, 4, self->widgets.entry_objextra);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 0, 5, self->widgets.label_objprob);
	liwdg_group_set_child (LIWDG_GROUP (group_attr), 1, 5, self->widgets.scroll_objprob);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_view), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self->widgets.group_view), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_view), 0, 0, self->widgets.group_paint);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_view), 0, 2, group_attr);
	liwdg_widget_set_visible (self->widgets.label_size, 0);
	liwdg_widget_set_visible (self->widgets.group_size, 0);

	/* Pack self. */
	liwdg_group_set_margins (LIWDG_GROUP (self), 5, 5, 5, 5);
	liwdg_group_set_spacings (LIWDG_GROUP (self), 5, 5);
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 1, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, group_tree);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 0, self->widgets.group_view);

	self->transform = limat_transform_identity ();
	self->paint = LIEXT_PREVIEW_INSERT_VOXEL;

	return 1;
}

static void
private_free (LIExtBrushes* self)
{
	if (self->clipboard != NULL)
		ligen_brush_free (self->clipboard);
	liwdg_tree_foreach (LIWDG_TREE (self->widgets.tree), free);
}

static int
private_event (LIExtBrushes* self,
               liwdgEvent*   event)
{
	return liwdg_widget_group.event (LIWDG_WIDGET (self), event);
}

static int
private_add (LIExtBrushes* self,
             LIWdgWidget*  widget)
{
	LIExtBrushesTreerow* data;
	LIGenBrush* brush;
	LIGenRule* rule;
	LIMatTransform transform;
	LIWdgTreerow* row;

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
private_copy (LIExtBrushes* self,
              LIWdgWidget*  widget)
{
	LIExtBrushesTreerow* data;
	LIGenRule* rule;
	LIWdgTreerow* row;

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
		case LIEXT_BRUSHES_ROWTYPE_OBJECTS:
		case LIEXT_BRUSHES_ROWTYPE_RULES:
			break;
		case LIEXT_BRUSHES_ROWTYPE_BRUSH:
			/* TODO */
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECT:
			/* TODO */
			break;
		case LIEXT_BRUSHES_ROWTYPE_STROKE:
			/* TODO */
			break;
		case LIEXT_BRUSHES_ROWTYPE_RULE:
			rule = ligen_rule_new_copy (data->rule);
			if (rule == NULL)
				return 0;
			if (!ligen_brush_insert_rule (data->brush, rule))
			{
				ligen_rule_free (rule);
				return 0;
			}
			row = liwdg_treerow_get_parent (row);
			private_append_rule (self, row, rule);
			private_rebuild_selection (self);
			break;
	}

	return 0;
}

static int
private_object_info (LIExtBrushes* self)
{
	char* tmp;
	LIExtBrushesTreerow* data;
	LIGenBrushobject* object;
	LIWdgTreerow* row;

	/* Get active row. */
	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return 0;
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);
	assert (data->type == LIEXT_BRUSHES_ROWTYPE_OBJECT);
	object = data->brush->objects.array[data->object];

	/* Update object info. */
	tmp = listr_dup (liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_objextra)));
	if (tmp != NULL)
	{
		lisys_free (object->extra);
		object->extra = tmp;
	}
	tmp = listr_dup (liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_objtype)));
	if (tmp != NULL)
	{
		lisys_free (object->type);
		object->type = tmp;
	}
	object->probability = liwdg_scroll_get_value (LIWDG_SCROLL (self->widgets.scroll_objprob));

	return 1;
}

static int
private_paint_select (LIExtBrushes* self,
                      LIWdgWidget*  widget)
{
	int i;

	for (i = 0 ; i < sizeof (self->widgets.button_paint) / sizeof (LIWdgWidget*) ; i++)
	{
		if (widget == self->widgets.button_paint[i])
		{
			self->paint = i;
			break;
		}
	}
	self->select.index = 0;

	return 0;
}

static int
private_paint_terrain (LIExtBrushes* self,
                       int           x,
                       int           y)
{
	int i;
	int tmp;
	int border;
	LIGenBrush* brush;
	LIMatVector line[2];
	LIMatVector point[2];
	LIExtPreview* preview;
	LIExtBrushesTreerow* data;
	LIMatAabb aabb;
	LIWdgTreerow* row;
	LIRenSelection result;

	/* Get active row. */
	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return 0;
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);
	if (data->type != LIEXT_BRUSHES_ROWTYPE_BRUSH)
		return 0;

	/* Pick from the scene. */
	/* Selects far intersection point with the box if no polygon hit. */
	if (!liwdg_render_pick (LIWDG_RENDER (self->widgets.preview), &result, x, y))
	{
		preview = LIEXT_PREVIEW (self->widgets.preview);
		point[0] = limat_vector_init (x, y, 0.0f);
		point[1] = limat_vector_init (x, y, 0.5f);
		lialg_camera_unproject (preview->camera, point + 0, line + 0);
		lialg_camera_unproject (preview->camera, point + 1, line + 1);
		liext_preview_get_bounds (preview, &aabb);
		if (!limat_intersect_aabb_line_far (&aabb, line + 0, line + 1, point))
			return 0;
		if (LIMAT_ABS (point->x - aabb.min.x) < LIMAT_EPSILON) point->x += 0.5;
		if (LIMAT_ABS (point->y - aabb.max.y) < LIMAT_EPSILON) point->y -= 0.5;
		if (LIMAT_ABS (point->y - aabb.min.y) < LIMAT_EPSILON) point->y += 0.5;
		if (LIMAT_ABS (point->y - aabb.max.y) < LIMAT_EPSILON) point->y -= 0.5;
		if (LIMAT_ABS (point->z - aabb.min.z) < LIMAT_EPSILON) point->z += 0.5;
		if (LIMAT_ABS (point->z - aabb.max.z) < LIMAT_EPSILON) point->z -= 0.5;
		result.point = *point;
		border = 1;
	}
	else
		border = 0;

	/* Paint terrain. */
	if (self->paint == LIEXT_PREVIEW_COPY_VOXEL)
	{
		if (!border)
			livox_manager_find_voxel (self->generator->voxels, LIVOX_FIND_FULL, &result.point, point);
		else
			*point = result.point;
		if (!self->select.index)
		{
			self->select.start[0] = point->x / LIVOX_TILE_WIDTH;
			self->select.start[1] = point->y / LIVOX_TILE_WIDTH;
			self->select.start[2] = point->z / LIVOX_TILE_WIDTH;
		}
		else
		{
			self->select.end[0] = point->x / LIVOX_TILE_WIDTH;
			self->select.end[1] = point->y / LIVOX_TILE_WIDTH;
			self->select.end[2] = point->z / LIVOX_TILE_WIDTH;
		}
		if (++self->select.index == 2)
		{
			for (i = 0 ; i < 3 ; i++)
			{
				if (self->select.start[i] > self->select.end[i])
				{
					tmp = self->select.start[i];
					self->select.start[i] = self->select.end[i];
					self->select.end[i] = tmp;
				}
			}
			brush = ligen_brush_new (self->generator,
				self->select.end[0] - self->select.start[0] + 1,
				self->select.end[1] - self->select.start[1] + 1,
				self->select.end[2] - self->select.start[2] + 1);
			if (brush == NULL)
				return 0;
			livox_manager_copy_voxels (self->generator->voxels,
				self->select.start[0], self->select.start[1], self->select.start[2],
				self->select.end[0] - self->select.start[0] + 1,
				self->select.end[1] - self->select.start[1] + 1,
				self->select.end[2] - self->select.start[2] + 1,
				brush->voxels.array);
			if (self->clipboard != NULL)
				ligen_brush_free (self->clipboard);
			self->clipboard = brush;
			self->select.index = 0;
		}
	}
	else if (self->paint == LIEXT_PREVIEW_PASTE_VOXEL)
	{
		if (self->clipboard != NULL)
		{
			if (!border)
				livox_manager_find_voxel (self->generator->voxels, LIVOX_FIND_EMPTY, &result.point, point);
			else
				*point = result.point;
			self->select.start[0] = point->x / LIVOX_TILE_WIDTH;
			self->select.start[1] = point->y / LIVOX_TILE_WIDTH;
			self->select.start[2] = point->z / LIVOX_TILE_WIDTH;
			livox_manager_paste_voxels (self->generator->voxels,
				self->select.start[0], self->select.start[1], self->select.start[2],
				self->clipboard->size[0], self->clipboard->size[1], self->clipboard->size[2],
				self->clipboard->voxels.array);
			liext_preview_copy_voxels (LIEXT_PREVIEW (self->widgets.preview), 0, 0, 0,
				data->brush->size[0], data->brush->size[1], data->brush->size[2],
				data->brush->voxels.array);
			private_rebuild_preview (self);
		}
	}
	else
	{
		liext_preview_paint_terrain (LIEXT_PREVIEW (self->widgets.preview), &result.point, self->paint,
			liext_materials_get_active (LIEXT_MATERIALS (LIEXT_EDITOR (self->module->editor)->materials)),
			liwdg_spin_get_value (LIWDG_SPIN (self->widgets.spin_axis)));
		liext_preview_copy_voxels (LIEXT_PREVIEW (self->widgets.preview), 0, 0, 0,
			data->brush->size[0], data->brush->size[1], data->brush->size[2],
			data->brush->voxels.array);
		private_rebuild_preview (self);
	}

	return 0;
}

static int
private_remove (LIExtBrushes* self,
                LIWdgWidget*  widget)
{
	int c;
	int i;
	LIWdgTreerow* row;
	LIWdgTreerow* rows[2];
	LIExtBrushesTreerow* data[3];

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
			lisys_free (data[0]);
			private_rebuild_selection (self);
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECTS:
			break;
		case LIEXT_BRUSHES_ROWTYPE_OBJECT:
			rows[0] = liwdg_treerow_get_parent (row);
			liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (row));
			ligen_brush_remove_object (data[0]->brush, data[0]->object);
			lisys_free (data[0]);
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
			lisys_free (data[0]);
			private_rebuild_selection (self);
			break;
		case LIEXT_BRUSHES_ROWTYPE_STROKE:
			rows[0] = liwdg_treerow_get_parent (row);
			liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (row));
			ligen_rule_remove_stroke (data[0]->rule, data[0]->stroke);
			lisys_free (data[0]);
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
private_rename (LIExtBrushes* self,
                LIWdgWidget*  widget)
{
	char* tmp;
	const char* name;
	LIAlgU32dicIter iter;
	LIGenBrush* brush;
	LIGenBrushobject* object;
	LIExtBrushesTreerow* data;
	LIWdgTreerow* row;

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
			tmp = listr_dup (name);
			if (tmp == NULL)
				return 0;
			object = data->brush->objects.array[data->object];
			lisys_free (object->model);
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
			LIALG_U32DIC_FOREACH (iter, self->generator->brushes)
			{
				brush = iter.value;
				if (!strcmp (name, brush->name))
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
private_selected (LIExtBrushes* self,
                  LIWdgWidget*  widget,
                  LIWdgTreerow* row)
{
	LIExtBrushesTreerow* data;
	LIWdgTreerow* row0;

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

static int
private_stroke_flags (LIExtBrushes* self)
{
	LIWdgTreerow* row;
	LIGenRulestroke* stroke;
	LIExtBrushesTreerow* data;

	/* Get active row. */
	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return 0;
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);
	assert (data->type == LIEXT_BRUSHES_ROWTYPE_STROKE);
	stroke = data->rule->strokes.array + data->stroke;

	/* Update stroke flags. */
	if (liwdg_check_get_active (LIWDG_CHECK (self->widgets.check_required)))
		stroke->flags |= LIGEN_RULE_REQUIRE;
	else
		stroke->flags &= ~LIGEN_RULE_REQUIRE;

	return 0;
}

static LIWdgTreerow*
private_append_brush (LIExtBrushes* self,
                      LIGenBrush*   brush)
{
	int i;
	LIGenBrushobject* object;
	LIGenRule* rule;
	LIWdgTreerow* rows[5] = { NULL, NULL, NULL, NULL, NULL };
	LIExtBrushesTreerow* data[5] = { NULL, NULL, NULL, NULL, NULL };

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
	data[1] = lisys_calloc (1, sizeof (LIExtBrushesTreerow));
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
	data[2] = lisys_calloc (1, sizeof (LIExtBrushesTreerow));
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
	data[3] = lisys_calloc (1, sizeof (LIExtBrushesTreerow));
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
				lisys_free (data[4]);
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
				lisys_free (data[4]);
				liwdg_treerow_remove_row (rows[2], 0);
			}
			while (liwdg_treerow_get_row_count (rows[3]))
			{
				rows[4] = liwdg_treerow_get_row (rows[3], 0);
				data[4] = liwdg_treerow_get_data (rows[4]);
				lisys_free (data[4]);
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
	lisys_free (data[0]);
	lisys_free (data[1]);
	lisys_free (data[2]);
	if (rows[1] != NULL)
		liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (rows[1]));
	return NULL;
}

static LIWdgTreerow*
private_append_object (LIExtBrushes* self,
                       LIWdgTreerow* row,
                       int           index)
{
	LIGenBrushobject* object;
	LIExtBrushesTreerow* data[2];
	LIWdgTreerow* rows[2];

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
	data[1] = lisys_calloc (1, sizeof (LIExtBrushesTreerow));
	if (data[1] == NULL)
		return NULL;
	data[1]->type = LIEXT_BRUSHES_ROWTYPE_OBJECT;
	data[1]->brush = data[0]->brush;
	data[1]->rule = NULL;
	data[1]->object = index;
	data[1]->stroke = -1;
	rows[1] = liwdg_treerow_append_row (rows[0], object->model, data[1]);
	if (rows[1] == NULL)
	{
		lisys_free (data[1]);
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

static LIWdgTreerow*
private_append_rule (LIExtBrushes* self,
                     LIWdgTreerow* row,
                     LIGenRule*    rule)
{
	int i;
	LIExtBrushesTreerow* data[2];
	LIWdgTreerow* rows[3];

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
	data[1] = lisys_calloc (1, sizeof (LIExtBrushesTreerow));
	if (data[1] == NULL)
		return NULL;
	data[1]->type = LIEXT_BRUSHES_ROWTYPE_RULE;
	data[1]->brush = data[0]->brush;
	data[1]->rule = rule;
	data[1]->object = -1;
	data[1]->stroke = -1;
	rows[1] = liwdg_treerow_append_row (rows[0], rule->name, data[1]);
	if (rows[1] == NULL)
	{
		lisys_free (data[1]);
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
				lisys_free (data[2]);
				liwdg_treerow_remove_row (rows[1], 0);
			}
			liwdg_treerow_remove_row (rows[0], liwdg_treerow_get_index (rows[1]));
			lisys_free (data[1]);
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

static LIWdgTreerow*
private_append_stroke (LIExtBrushes* self,
                       LIWdgTreerow* row,
                       int           index)
{
	LIExtBrushesTreerow* data[2];
	LIGenBrush* brush;
	LIGenRulestroke* stroke;
	LIWdgTreerow* rows[2];

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
	data[1] = lisys_calloc (1, sizeof (LIExtBrushesTreerow));
	if (data[1] == NULL)
		return NULL;
	data[1]->type = LIEXT_BRUSHES_ROWTYPE_STROKE;
	data[1]->brush = data[0]->brush;
	data[1]->rule = data[0]->rule;
	data[1]->object = -1;
	data[1]->stroke = index;
	rows[1] = liwdg_treerow_append_row (rows[0], brush->name, data[1]);
	if (rows[1] == NULL)
	{
		lisys_free (data[1]);
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
private_remove_strokes (LIExtBrushes* self,
                        LIWdgTreerow* row,
                        int           brush)
{
	int c;
	int i;
	LIExtBrushesTreerow* data;
	LIWdgTreerow* row1;

	c = liwdg_treerow_get_row_count (row);
	for (i = 0 ; i < c ; i++)
	{
		row1 = liwdg_treerow_get_row (row, i);
		data = liwdg_treerow_get_data (row1);
		if (data->type == LIEXT_BRUSHES_ROWTYPE_STROKE &&
		    data->rule->strokes.array[data->stroke].brush == brush)
		{
			liwdg_treerow_remove_row (row, i);
			lisys_free (data);
			i--;
			c--;
		}
		else
			private_remove_strokes (self, row1, brush);
	}
}

static void
private_rename_strokes (LIExtBrushes* self,
                        LIWdgTreerow* row,
                        int           brush,
                        const char*   text)
{
	int c;
	int i;
	LIExtBrushesTreerow* data;
	LIWdgTreerow* row1;

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
private_transform (LIExtBrushes*   self,
                   LIMatTransform* transform,
                   int             finish)
{
	LIGenBrushobject* object;
	LIGenRulestroke* stroke;
	LIExtBrushesTreerow* row;
	LIMatQuaternion quat;
	LIMatVector pos;

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
private_populate (LIExtBrushes* self)
{
	LIAlgU32dicIter iter;
	LIExtBrushesTreerow* data;
	LIGenBrush* brush;
	LIWdgTreerow* row;

	/* Clear tree. */
	row = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
	liwdg_tree_foreach (LIWDG_TREE (self->widgets.tree), free);
	liwdg_tree_clear (LIWDG_TREE (self->widgets.tree));

	/* Add root row. */
	data = lisys_calloc (1, sizeof (LIExtBrushesTreerow));
	if (data == NULL)
		return;
	data->type = LIEXT_BRUSHES_ROWTYPE_ROOT;
	data->brush = NULL;
	data->rule = NULL;
	data->object = -1;
	data->stroke = -1;
	if (!liwdg_treerow_append_row (row, "Brushes", data))
	{
		lisys_free (data);
		return;
	}

	/* Append brushes. */
	LIALG_U32DIC_FOREACH (iter, self->generator->brushes)
	{
		brush = iter.value;
		private_append_brush (self, brush);
	}
}

static void
private_resize_brush (LIExtBrushes* self)
{
	int x;
	int y;
	int z;
	LIExtBrushesTreerow* data;
	LIWdgTreerow* row;

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
	x = LIMAT_MAX (1, x);
	y = LIMAT_MAX (1, y);
	z = LIMAT_MAX (1, z);
	ligen_brush_set_size (data->brush, x, y, z);

	/* Rebuild preview. */
	private_rebuild_preview (self);
}

static void
private_rebuild_brush (LIExtBrushes* self)
{
	int x;
	int y;
	int z;
	LIEngObject* player;
	LIExtBrushesTreerow* data;
	LIGenBrush* brush;
	LIMatTransform transform;
	LIMatTransform transform0;
	LIMatTransform transform1;
	LIWdgTreerow* row;

	/* Find the brush. */
	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return;
	data = liwdg_treerow_get_data (row);
	assert (data != NULL);
	assert (data->brush != NULL); 
	brush = data->brush;

	/* Find the player. */
	if (self->module->client->network == NULL)
		return;
	player = licli_client_get_player (self->module->client);
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
#warning Brush rebuilding is broken.
#if 0
	livox_manager_copy_voxels (self->module->client->voxels, x, y, z,
		brush->size[0], brush->size[1], brush->size[2], brush->voxels.array);
#endif

	/* Rebuild preview. */
	private_rebuild_preview (self);
}

static void
private_rebuild_preview (LIExtBrushes* self)
{
	int i;
	int j;
	int pos[3];
	LIExtBrushesTreerow* row;
	LIGenBrush* brush;
	LIGenBrushobject* object;
	LIGenRulestroke* stroke;
	LIMatQuaternion quat;
	LIMatTransform transform;
	LIMatVector vec;

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
private_rebuild_selection (LIExtBrushes* self)
{
	int show;
	LIExtBrushesTreerow* data;
	LIGenBrush* brush;
	LIGenBrushobject* object;
	LIGenRulestroke* stroke;
	LIWdgTreerow* row;

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
		liwdg_widget_set_visible (self->widgets.check_required, 0);
		liwdg_widget_set_visible (self->widgets.label_size, 0);
		liwdg_widget_set_visible (self->widgets.group_size, 0);
	}
	else if (data->object >= 0)
	{
		object = data->brush->objects.array[data->object];
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Object:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), object->model);
		liwdg_widget_set_visible (self->widgets.check_required, 0);
		liwdg_widget_set_visible (self->widgets.label_size, 0);
		liwdg_widget_set_visible (self->widgets.group_size, 0);
	}
	else if (data->rule == NULL)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Brush:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), data->brush->name);
		liwdg_widget_set_visible (self->widgets.check_required, 0);
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
		liwdg_widget_set_visible (self->widgets.check_required, 0);
		liwdg_widget_set_visible (self->widgets.label_size, 0);
		liwdg_widget_set_visible (self->widgets.group_size, 0);
	}
	else
	{
		stroke = data->rule->strokes.array + data->stroke;
		brush = ligen_generator_find_brush (self->generator, stroke->brush);
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Stroke:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), brush != NULL? brush->name : "");
		liwdg_widget_set_visible (self->widgets.check_required, 1);
		liwdg_check_set_active (LIWDG_CHECK (self->widgets.check_required), stroke->flags & LIGEN_RULE_REQUIRE);
		liwdg_widget_set_visible (self->widgets.label_size, 0);
		liwdg_widget_set_visible (self->widgets.group_size, 0);
	}

	/* Object info. */
	show = (data->type == LIEXT_BRUSHES_ROWTYPE_OBJECT);
	liwdg_widget_set_visible (self->widgets.label_objextra, show);
	liwdg_widget_set_visible (self->widgets.label_objprob, show);
	liwdg_widget_set_visible (self->widgets.label_objtype, show);
	liwdg_widget_set_visible (self->widgets.entry_objextra, show);
	liwdg_widget_set_visible (self->widgets.entry_objtype, show);
	liwdg_widget_set_visible (self->widgets.scroll_objprob, show);
	if (show)
	{
		object = data->brush->objects.array[data->object];
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_objextra), object->extra);
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_objtype), object->type);
		liwdg_scroll_set_value (LIWDG_SCROLL (self->widgets.scroll_objprob), object->probability);
	}

	/* Rebuild preview. */
	private_rebuild_preview (self);
}

static LIExtBrushesTreerow*
private_get_active (LIExtBrushes* self)
{
	LIWdgTreerow* row;

	row = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL)
		return NULL;
	return liwdg_treerow_get_data (row);
}

/** @} */
/** @} */
/** @} */
