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
#include "ext-materials.h"
#include "ext-preview.h"

static const void*
private_base ();

static int
private_init (LIExtMaterials* self,
              LIWdgManager*   manager);

static void
private_free (LIExtMaterials* self);

static int
private_event (LIExtMaterials* self,
               LIWdgEvent*     event);

static int
private_add (LIExtMaterials* self,
             LIWdgWidget*    widget);

static int
private_remove (LIExtMaterials* self,
                LIWdgWidget*    widget);

static int
private_changed_friction (LIExtMaterials* self,
                          LIWdgWidget*    widget);

static int
private_changed_model (LIExtMaterials* self,
                       LIWdgWidget*    widget);

static int
private_changed_name (LIExtMaterials* self,
                      LIWdgWidget*    widget);

static int
private_selected (LIExtMaterials* self,
                  LIWdgTreerow*   row);

static void
private_populate (LIExtMaterials* self);

static void
private_rebuild_preview (LIExtMaterials* self);

static void
private_rebuild_selection (LIExtMaterials* self,
                           LIWdgTreerow*   row);

static LIExtMaterialsTreerow*
private_get_active (LIExtMaterials* self);

/****************************************************************************/

const LIWdgClass liext_widget_materials =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorMaterials", sizeof (LIExtMaterials),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event
};

LIWdgWidget*
liext_materials_new (LIWdgManager* manager,
                     LIExtModule*  module)
{
	LIExtMaterials* data;
	LIWdgWidget* self;

	self = liwdg_widget_new (manager, &liext_widget_materials);
	if (self == NULL)
		return NULL;
	data = LIEXT_MATERIALS (self);
	data->module = module;

	/* Initialize scene preview. */
	data->widgets.preview = liext_preview_new (manager, module->client);
	if (data->widgets.preview == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liwdg_widget_set_request (data->widgets.preview, 128, 128);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, data->widgets.preview);
	data->generator = LIEXT_PREVIEW (data->widgets.preview)->generator;

	/* Populate list. */
	private_populate (data);

	return self;
}

int
liext_materials_save (LIExtMaterials* self)
{
	return ligen_generator_write_materials (self->generator);
}

void
liext_materials_reset (LIExtMaterials* self,
                       LIArcReader*    reader)
{
	reader->pos = 1;
	liext_preview_replace_materials (LIEXT_PREVIEW (self->widgets.preview), reader);
	private_populate (self);
}

int
liext_materials_get_active (LIExtMaterials* self)
{
	LIExtMaterialsTreerow* row;

	row = private_get_active (self);
	if (row == NULL)
		return 0;
	return row->material->id;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdg_widget_group;
}

static int
private_init (LIExtMaterials* self,
              LIWdgManager*   manager)
{
	LIWdgWidget* group_tree;

	/* Allocate widgets. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 1, 3))
		return 0;
	if (!liwdg_manager_alloc_widgets (manager,
		&group_tree, liwdg_group_new_with_size (manager, 1, 3),
		&self->widgets.group_attr, liwdg_group_new_with_size (manager, 2, 3),
		&self->widgets.group_view, liwdg_group_new_with_size (manager, 1, 1),
		&self->widgets.button_add, liwdg_button_new (manager),
		&self->widgets.button_remove, liwdg_button_new (manager),
		&self->widgets.entry_name, liwdg_entry_new (manager),
		&self->widgets.entry_model, liwdg_entry_new (manager),
		&self->widgets.label_name, liwdg_label_new (manager),
		&self->widgets.label_model, liwdg_label_new (manager),
		&self->widgets.label_friction, liwdg_label_new (manager),
		&self->widgets.scroll_friction, liwdg_scroll_new (manager),
		&self->widgets.tree, liwdg_tree_new (manager),
		&self->widgets.view, liwdg_view_new (manager), NULL))
		return 0;

	/* Configure widgets. */
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_add), "Add");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_remove), "Remove");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_friction), "Friction:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_model), "Model:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_name), "Name:");
	liwdg_widget_insert_callback (self->widgets.button_add, "pressed", private_add, self);
	liwdg_widget_insert_callback (self->widgets.button_remove, "pressed", private_remove, self);
	liwdg_widget_insert_callback (self->widgets.entry_model, "edited", private_changed_model, self);
	liwdg_widget_insert_callback (self->widgets.entry_name, "edited", private_changed_name, self);
	liwdg_widget_insert_callback (self->widgets.tree, "pressed", private_selected, self);
	liwdg_widget_insert_callback (self->widgets.scroll_friction, "pressed", private_changed_friction, self);

	/* Tree. */
	liwdg_view_set_vscroll (LIWDG_VIEW (self->widgets.view), 1);
	liwdg_view_set_child (LIWDG_VIEW (self->widgets.view), self->widgets.tree);
	liwdg_group_set_row_expand (LIWDG_GROUP (group_tree), 0, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_tree), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 0, self->widgets.view);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 1, self->widgets.button_remove);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 2, self->widgets.button_add);

	/* Preview. */
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_attr), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_attr), 0, 0, self->widgets.label_name);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_attr), 1, 0, self->widgets.entry_name);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_attr), 0, 1, self->widgets.label_model);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_attr), 1, 1, self->widgets.entry_model);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_attr), 0, 2, self->widgets.label_friction);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_attr), 1, 2, self->widgets.scroll_friction);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_view), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_view), 0, 0, self->widgets.group_attr);
	liwdg_widget_set_visible (self->widgets.group_attr, 0);

	/* Pack self. */
	liwdg_group_set_margins (LIWDG_GROUP (self), 5, 5, 5, 5);
	liwdg_group_set_spacings (LIWDG_GROUP (self), 5, 5);
	liwdg_group_set_row_expand (LIWDG_GROUP (self), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 1, group_tree);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 2, self->widgets.group_view);

	return 1;
}

static void
private_free (LIExtMaterials* self)
{
	liwdg_tree_foreach (LIWDG_TREE (self->widgets.tree), free);
}

static int
private_event (LIExtMaterials* self,
               LIWdgEvent*     event)
{
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
	}

	return liwdg_widget_group.event (LIWDG_WIDGET (self), event);
}

static int
private_add (LIExtMaterials* self,
             LIWdgWidget*    widget)
{
	int i;
	LIExtMaterialsTreerow* row;
	LIVoxMaterial* material;
	LIWdgTreerow* trow[2];

	/* Allocate material. */
	material = livox_material_new ();
	if (material == NULL)
		return 0;
	for (i = 1 ; i < 256 ; i++)
	{
		if (livox_manager_find_material (self->generator->voxels, i) == NULL)
			break;
	}
	if (i == 256)
		return 0;
	material->id = i;

	/* Add to voxel manager. */
	if (!livox_manager_insert_material (self->generator->voxels, material))
	{
		livox_material_free (material);
		return 0;
	}

	/* Add to tree view. */
	row = lisys_calloc (1, sizeof (LIExtMaterialsTreerow));
	if (row == NULL)
		return 0;
	row->material = material;
	trow[0] = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
	trow[1] = liwdg_treerow_append_row (trow[0], material->name, row);
	if (trow[1] == NULL)
	{
		lisys_free (row);
		return 0;
	}

	/* Set active. */
	trow[0] = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (trow[0] != NULL)
		liwdg_treerow_set_highlighted (trow[0], 0);
	liwdg_treerow_set_highlighted (trow[1], 1);
	private_rebuild_selection (self, trow[1]);

	return 0;
}

static int
private_remove (LIExtMaterials* self,
                LIWdgWidget*    widget)
{
	LIExtMaterialsTreerow* row;
	LIWdgTreerow* trow;

	/* Get active item. */
	row = private_get_active (self);
	if (row == NULL || row->material == NULL)
		return 0;

	/* Remove the selected item. */
	livox_manager_remove_material (self->generator->voxels, row->material->id);

	/* Remove the row. */
	trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	lisys_free (liwdg_treerow_get_data (trow));
	liwdg_treerow_remove_row (
		liwdg_treerow_get_parent (trow),
		liwdg_treerow_get_index (trow));

	private_rebuild_preview (self);

	return 0;
}

static int
private_changed_friction (LIExtMaterials* self,
                          LIWdgWidget*    widget)
{
	LIExtMaterialsTreerow* row;

	row = private_get_active (self);
	assert (row != NULL);
	assert (row->material != NULL);

	row->material->friction = liwdg_scroll_get_value (LIWDG_SCROLL (widget));
	private_rebuild_preview (self);

	return 0;
}

static int
private_changed_model (LIExtMaterials* self,
                       LIWdgWidget*    widget)
{
	const char* model;
	LIExtMaterialsTreerow* row;
	LIWdgTreerow* trow;

	row = private_get_active (self);
	trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL || row->material == NULL)
		return 0;

	/* Material. */
	model = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_model));
	livox_material_set_model (row->material, model);
	private_rebuild_preview (self);

	return 0;
}

static int
private_changed_name (LIExtMaterials* self,
                      LIWdgWidget*    widget)
{
	const char* name;
	LIExtMaterialsTreerow* row;
	LIWdgTreerow* trow;

	row = private_get_active (self);
	trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL || row->material == NULL)
		return 0;

	/* Material. */
	name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
	livox_material_set_name (row->material, name);
	liwdg_treerow_set_text (trow, name);

	return 0;
}

static int
private_selected (LIExtMaterials* self,
                  LIWdgTreerow*   row)
{
	LIWdgTreerow* row0;

	/* Deselect old. */
	row0 = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row0 != NULL)
		liwdg_treerow_set_highlighted (row0, 0);

	/* Select new. */
	liwdg_treerow_set_highlighted (row, 1);
	private_rebuild_selection (self, row);

	return 0;
}

static void
private_populate (LIExtMaterials* self)
{
	LIAlgU32dicIter iter;
	LIExtMaterialsTreerow* tmp;
	LIVoxMaterial* material;
	LIWdgTreerow* tree;
	LIWdgTreerow* row0;
	LIWdgTreerow* row1;

	/* Clear tree. */
	tree = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
	liwdg_tree_foreach (LIWDG_TREE (self->widgets.tree), free);
	liwdg_tree_clear (LIWDG_TREE (self->widgets.tree));

	/* Add root info. */
	tmp = lisys_calloc (1, sizeof (LIExtMaterialsTreerow));
	if (tmp == NULL)
		return;
	tmp->material = NULL;
	row0 = tree;

	/* Add each material. */
	LIALG_U32DIC_FOREACH (iter, self->generator->voxels->materials)
	{
		material = iter.value;
		tmp = lisys_calloc (1, sizeof (LIExtMaterialsTreerow));
		tmp->material = material;
		row1 = liwdg_treerow_append_row (row0, material->name, tmp);
		if (row1 == NULL)
			lisys_free (tmp);
	}
}

static void
private_rebuild_preview (LIExtMaterials* self)
{
	LIExtMaterialsTreerow* row;
	LIMatVector eye;
	LIMatVector ctr;
	LIMatVector up;

	row = private_get_active (self);
	if (row == NULL || row->material == NULL)
	{
		liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
		liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
	}
	else
	{
		liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
		liext_preview_build_tile (LIEXT_PREVIEW (self->widgets.preview), row->material->id);
		eye = limat_vector_init (0.0f, 0.0f, 0.0f);
		ctr = limat_vector_init (0.0f, 0.0f, 0.0f);
		up = limat_vector_init (0.0f, 1.0f, 0.0f);
		LIEXT_PREVIEW (self->widgets.preview)->camera->config.distance = 6.0f;
		liext_preview_setup_camera (LIEXT_PREVIEW (self->widgets.preview),
			&eye, &ctr, &up, LIALG_CAMERA_THIRDPERSON);
	}
}

static void
private_rebuild_selection (LIExtMaterials* self,
                           LIWdgTreerow*   row)
{
	LIExtMaterialsTreerow* data;

	/* Set info. */
	data = liwdg_treerow_get_data (row);
	if (data == NULL || data->material == NULL)
	{
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), "");
		liwdg_widget_set_visible (self->widgets.group_attr, 0);
	}
	else
	{
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), data->material->name);
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_model), data->material->model);
		liwdg_widget_set_visible (self->widgets.group_attr, 1);
		liwdg_scroll_set_value (LIWDG_SCROLL (self->widgets.scroll_friction), data->material->friction);
	}

	/* Rebuild preview. */
	private_rebuild_preview (self);
}

static LIExtMaterialsTreerow*
private_get_active (LIExtMaterials* self)
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
