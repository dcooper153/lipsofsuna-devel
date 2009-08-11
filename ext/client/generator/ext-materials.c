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
#include "ext-materials.h"
#include "ext-preview.h"

static const void*
private_base ();

static int
private_init (liextMaterials* self,
              liwdgManager*   manager);

static void
private_free (liextMaterials* self);

static int
private_event (liextMaterials* self,
               liwdgEvent*     event);

static int
private_add (liextMaterials* self,
             liwdgWidget*    widget);

static int
private_remove (liextMaterials* self,
                liwdgWidget*    widget);

static int
private_rename (liextMaterials* self,
                liwdgWidget*    widget);

static int
private_scaled_material (liextMaterials* self,
                         liwdgWidget*    widget);

static int
private_selected (liextMaterials* self,
                  liwdgWidget*    widget,
                  liwdgTreerow*   row);

static void
private_populate (liextMaterials* self);

static void
private_rebuild_preview (liextMaterials* self);

static liextMaterialsTreerow*
private_get_active (liextMaterials* self);

/****************************************************************************/

const liwdgClass liextMaterialsType =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorMaterials", sizeof (liextMaterials),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_materials_new (liwdgManager* manager,
                     liextModule*  module)
{
	liextMaterials* data;
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liextMaterialsType);
	if (self == NULL)
		return NULL;
	data = LIEXT_MATERIALS (self);
	data->module = module;

	/* Initialize scene preview. */
	data->widgets.preview = liext_preview_new (manager, module->module);
	if (data->widgets.preview == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liwdg_widget_set_request (data->widgets.preview, 320, 240);
	liwdg_group_set_child (LIWDG_GROUP (data->widgets.group_view), 0, 3, data->widgets.preview);
	data->generator = LIEXT_PREVIEW (data->widgets.preview)->generator;

	/* Populate list. */
	private_populate (data);

	return self;
}

int
liext_materials_save (liextMaterials* self)
{
	int ret;

	livox_manager_set_sql (self->generator->voxels, self->generator->srvsql);
	ret = livox_manager_write_materials (self->generator->voxels);
	livox_manager_set_sql (self->generator->voxels, NULL);

	return ret;
}

void
liext_materials_reset (liextMaterials* self)
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
private_init (liextMaterials* self,
              liwdgManager*   manager)
{
	int i;
	liwdgWidget* group_tree;
	liwdgWidget* widgets[] =
	{
		liwdg_group_new_with_size (manager, 1, 5),
		liwdg_group_new_with_size (manager, 2, 1),
		liwdg_group_new_with_size (manager, 2, 1),
		liwdg_group_new_with_size (manager, 1, 4),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_entry_new (manager),
		liwdg_label_new (manager),
		liwdg_label_new (manager),
		liwdg_scroll_new (manager),
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
	group_tree = widgets[(i = 0)];
	self->widgets.group_name = widgets[++i];
	self->widgets.group_scale = widgets[++i];
	self->widgets.group_view = widgets[++i];
	self->widgets.button_add = widgets[++i];
	self->widgets.button_remove = widgets[++i];
	self->widgets.entry_name = widgets[++i];
	self->widgets.label_type = widgets[++i];
	self->widgets.label_scale = widgets[++i];
	self->widgets.scroll_scale = widgets[++i];
	self->widgets.tree = widgets[++i];

	/* Configure widgets. */
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_add), "Add");
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_remove), "Remove");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_scale), "Scale:");
	liwdg_widget_insert_callback (self->widgets.button_add, LIWDG_CALLBACK_PRESSED, 0, private_add, self, NULL);
	liwdg_widget_insert_callback (self->widgets.button_remove, LIWDG_CALLBACK_PRESSED, 0, private_remove, self, NULL);
	liwdg_widget_insert_callback (self->widgets.entry_name, LIWDG_CALLBACK_EDITED, 0, private_rename, self, NULL);
	liwdg_widget_insert_callback (self->widgets.tree, LIWDG_CALLBACK_PRESSED, 0, private_selected, self, NULL);
	liwdg_widget_insert_callback (self->widgets.scroll_scale, LIWDG_CALLBACK_PRESSED, 0, private_scaled_material, self, NULL);

	/* Tree. */
	liwdg_group_set_row_expand (LIWDG_GROUP (group_tree), 2, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (group_tree), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 4, liwdg_label_new_with_text (manager, "Materials"));
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 3, self->widgets.tree);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 1, self->widgets.button_add);
	liwdg_group_set_child (LIWDG_GROUP (group_tree), 0, 0, self->widgets.button_remove);

	/* Preview. */
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_name), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_name), 0, 0, self->widgets.label_type);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_name), 1, 0, self->widgets.entry_name);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_scale), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_scale), 0, 0, self->widgets.label_scale);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_scale), 1, 0, self->widgets.scroll_scale);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->widgets.group_view), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self->widgets.group_view), 3, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_view), 0, 1, self->widgets.group_name);
	liwdg_group_set_child (LIWDG_GROUP (self->widgets.group_view), 0, 0, self->widgets.group_scale);
	liwdg_widget_set_visible (self->widgets.group_scale, 0);

	/* Pack self. */
	liwdg_group_set_spacings (LIWDG_GROUP (self), 5, 5);
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
private_free (liextMaterials* self)
{
	liwdg_tree_foreach (LIWDG_TREE (self->widgets.tree), free);
}

static int
private_event (liextMaterials* self,
               liwdgEvent*     event)
{
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
	}

	return liwdgGroupType.event (LIWDG_WIDGET (self), event);
}

static int
private_add (liextMaterials* self,
             liwdgWidget*    widget)
{
	int i;
	liextMaterialsTreerow* row;
	liextMaterialsTreerow* row1;
	livoxMaterial* material;
	liwdgTreerow* trow;

	/* Get active item. */
	row = private_get_active (self);
	trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));

	/* Material? */
	if (row == NULL || row->material == NULL)
	{
		material = livox_material_new ();
		if (material == NULL)
			return 0;
		for (i = 0 ; i < 256 ; i++)
		{
			if (livox_manager_find_material (self->generator->voxels, i) == NULL)
				break;
		}
		if (i == 256)
			return 0;
		material->id = i;
		if (!livox_manager_insert_material (self->generator->voxels, material))
		{
			livox_material_free (material);
			return 0;
		}
		trow = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
		row1 = calloc (1, sizeof (liextMaterialsTreerow));
		row1->material = material;
		row1->texture = -1;
		if (!liwdg_treerow_append_row (trow, material->name, row1))
			free (row1);
	}

	/* Texture? */
	else
	{
		if (!livox_material_append_texture (row->material, "empty"))
			return 0;
		row1 = calloc (1, sizeof (liextMaterialsTreerow));
		row1->material = row->material;
		row1->texture = row->material->model.textures.count - 1;
		if (!liwdg_treerow_append_row (trow, "empty", row1))
			free (row1);
	}

	return 0;
}

static int
private_remove (liextMaterials* self,
                liwdgWidget*    widget)
{
	liextMaterialsTreerow* row;
	liwdgTreerow* trow;

	/* Get active item. */
	row = private_get_active (self);
	if (row == NULL || row->material == NULL)
	{
#warning FIXME: Abusing this for material saving!
		liext_materials_save (self);
		return 0;
	}

	/* Remove the selected item. */
	if (row->texture < 0)
		livox_manager_remove_material (self->generator->voxels, row->material->id);
	else
		livox_material_remove_texture (row->material, row->texture);

	/* Remove the row. */
	trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	free (liwdg_treerow_get_data (trow));
	liwdg_treerow_remove_row (
		liwdg_treerow_get_parent (trow),
		liwdg_treerow_get_index (trow));

	private_rebuild_preview (self);

	return 0;
}

static int
private_rename (liextMaterials* self,
                liwdgWidget*    widget)
{
	const char* name;
	limdlTexture* texture;
	liextMaterialsTreerow* row;
	liwdgTreerow* trow;

	row = private_get_active (self);
	trow = liwdg_tree_get_active (LIWDG_TREE (self->widgets.tree));
	if (row == NULL || row->material == NULL)
		return 0;

	/* Material? */
	if (row->texture < 0)
	{
		name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
		livox_material_set_name (row->material, name);
		liwdg_treerow_set_text (trow, name);
	}

	/* Texture? */
	else
	{
		texture = row->material->model.textures.array + row->texture;
		name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
		limdl_texture_set_string (texture, name);
		liwdg_treerow_set_text (trow, name);
	}

	return 0;
}

static int
private_scaled_material (liextMaterials* self,
                         liwdgWidget*    widget)
{
	liextMaterialsTreerow* row;

	row = private_get_active (self);
	assert (row != NULL);
	assert (row->material != NULL);

	row->material->scale = liwdg_scroll_get_value (LIWDG_SCROLL (widget));
	private_rebuild_preview (self);

	return 0;
}

static int
private_selected (liextMaterials* self,
                  liwdgWidget*    widget,
                  liwdgTreerow*   row)
{
	liextMaterialsTreerow* data;
	limdlTexture* texture;
	liwdgTreerow* row0;

	/* Deselect old. */
	row0 = liwdg_tree_get_active (LIWDG_TREE (widget));
	if (row0 != NULL)
		liwdg_treerow_set_highlighted (row0, 0);

	/* Select new. */
	liwdg_treerow_set_highlighted (row, 1);
	data = liwdg_treerow_get_data (row);

	/* Set info. */
	if (data == NULL || data->material == NULL)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), "");
		liwdg_widget_set_visible (self->widgets.group_scale, 0);
	}
	else if (data->texture < 0)
	{
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Material:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), data->material->name);
		liwdg_widget_set_visible (self->widgets.group_scale, 1);
		liwdg_scroll_set_value (LIWDG_SCROLL (self->widgets.scroll_scale), data->material->scale);
	}
	else
	{
		texture = data->material->model.textures.array + data->texture;
		liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Texture:");
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), texture->string);
		liwdg_widget_set_visible (self->widgets.group_scale, 1);
		liwdg_scroll_set_value (LIWDG_SCROLL (self->widgets.scroll_scale), data->material->scale);
	}

	/* Rebuild preview. */
	private_rebuild_preview (self);

	return 0;
}

static void
private_populate (liextMaterials* self)
{
	int j;
	lialgU32dicIter iter;
	liextMaterialsTreerow* tmp;
	limdlTexture* texture;
	livoxMaterial* material;
	liwdgTreerow* tree;
	liwdgTreerow* row0;
	liwdgTreerow* row1;

	/* Clear tree. */
	tree = liwdg_tree_get_root (LIWDG_TREE (self->widgets.tree));
	liwdg_tree_foreach (LIWDG_TREE (self->widgets.tree), free);
	liwdg_tree_clear (LIWDG_TREE (self->widgets.tree));

	/* Add root info. */
	tmp = calloc (1, sizeof (liextMaterialsTreerow));
	tmp->material = NULL;
	tmp->texture = -1;
	liwdg_treerow_append_row (tree, "Root", tmp);

	/* Loop through materials. */
	LI_FOREACH_U32DIC (iter, self->generator->voxels->materials)
	{
		material = iter.value;

		/* Add material info. */
		tmp = calloc (1, sizeof (liextMaterialsTreerow));
		tmp->material = material;
		tmp->texture = -1;
		row0 = liwdg_treerow_append_row (tree, material->name, tmp);
		if (row0 == NULL)
			return;

		/* Loop through textures. */
		for (j = 0 ; j < material->model.textures.count ; j++)
		{
			texture = material->model.textures.array + j;

			/* Add texture info. */
			tmp = calloc (1, sizeof (liextMaterialsTreerow));
			tmp->material = material;
			tmp->texture = j;
			row1 = liwdg_treerow_append_row (row0, texture->string, tmp);
			if (row1 == NULL)
				return;
		}
	}
}

static void
private_rebuild_preview (liextMaterials* self)
{
	liextMaterialsTreerow* row;

	row = private_get_active (self);

	/* None? */
	if (row == NULL || row->material == NULL)
	{
		liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
		liext_preview_build (LIEXT_PREVIEW (self->widgets.preview));
	}

	/* Material? */
	else
	{
		liext_preview_clear (LIEXT_PREVIEW (self->widgets.preview));
		liext_preview_build_box (LIEXT_PREVIEW (self->widgets.preview), 10, 10, 10, row->material->id);
	}
}

static liextMaterialsTreerow*
private_get_active (liextMaterials* self)
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
