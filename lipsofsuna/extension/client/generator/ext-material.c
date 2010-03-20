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

#include "ext-module.h"
#include "ext-material.h"
#include "ext-preview.h"

static int
private_init (LIExtMaterial* self,
              LIWdgManager*  manager);

static void
private_free (LIExtMaterial* self);

static int
private_event (LIExtMaterial* self,
               LIWdgEvent*    event);

static int
private_pressed_occlude (LIExtMaterial* self,
                         LIWdgWidget*   widget);

static int
private_pressed_type (LIExtMaterial* self,
                      LIWdgWidget*   widget);

static int
private_changed_friction (LIExtMaterial* self,
                          LIWdgWidget*   widget);

static int
private_changed_model (LIExtMaterial* self,
                       LIWdgWidget*   widget);

static int
private_changed_name (LIExtMaterial* self,
                      LIWdgWidget*   widget);

static int
private_changed_shader (LIExtMaterial* self,
                        LIWdgWidget*   widget);

static int
private_changed_texture (LIExtMaterial* self,
                         LIWdgWidget*   widget);

static void
private_rebuild_preview (LIExtMaterial* self);

static void
private_rebuild_type (LIExtMaterial* self);

static char*
private_texture_string (LIExtMaterial* self,
                        LIMdlMaterial* material);

/****************************************************************************/

const LIWdgClass*
liext_widget_material ()
{
	static LIWdgClass clss =
	{
		NULL, "GeneratorMaterial", sizeof (LIExtMaterial),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	clss.base = liwdg_widget_group;
	return &clss;
}

LIWdgWidget*
liext_material_new (LIWdgManager* manager,
                    LIExtModule*  module,
                    LIExtPreview* preview)
{
	LIExtMaterial* data;
	LIWdgWidget* self;

	self = liwdg_widget_new (manager, liext_widget_material ());
	if (self == NULL)
		return NULL;
	data = LIEXT_MATERIAL (self);
	data->module = module;
	data->preview = preview;

	return self;
}

void
liext_material_set_material (LIExtMaterial* self,
                             LIVoxMaterial* material,
                             LIWdgTreerow*  treerow)
{
	char* name;

	/* Store values. */
	self->material = material;
	self->treerow = treerow;

	/* Populate widgets. */
	if (material != NULL)
	{
		lisys_assert (treerow != NULL);
		private_rebuild_type (self);
		liwdg_check_set_active (LIWDG_CHECK (self->widgets.check_occlude), material->flags & LIVOX_MATERIAL_FLAG_OCCLUDER);
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_name), material->name);
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_model), material->model);
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_shader1), material->mat_top.shader);
		liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_shader2), material->mat_side.shader);

		/* Set texture strings. */
		name = private_texture_string (self, &material->mat_top);
		if (name != NULL)
			liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_texture1), name);
		else
			liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_texture1), "");
		lisys_free (name);
		name = private_texture_string (self, &material->mat_side);
		if (name != NULL)
			liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_texture2), name);
		else
			liwdg_entry_set_text (LIWDG_ENTRY (self->widgets.entry_texture2), "");
		lisys_free (name);

		liwdg_scroll_set_value (LIWDG_SCROLL (self->widgets.scroll_friction), material->friction);
		liwdg_widget_set_visible (LIWDG_WIDGET (self), 1);
	}
	else
	{
		lisys_assert (treerow == NULL);
		liwdg_widget_set_visible (LIWDG_WIDGET (self), 0);
	}

	/* Rebuild preview. */
	private_rebuild_preview (self);
}

/****************************************************************************/

static int
private_init (LIExtMaterial* self,
              LIWdgManager*  manager)
{
	/* Allocate widgets. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 2, 9))
		return 0;
	if (!liwdg_manager_alloc_widgets (manager,
		&self->widgets.button_type, liwdg_button_new (manager),
		&self->widgets.check_occlude, liwdg_check_new (manager),
		&self->widgets.entry_name, liwdg_entry_new (manager),
		&self->widgets.entry_model, liwdg_entry_new (manager),
		&self->widgets.entry_shader1, liwdg_entry_new (manager),
		&self->widgets.entry_shader2, liwdg_entry_new (manager),
		&self->widgets.entry_texture1, liwdg_entry_new (manager),
		&self->widgets.entry_texture2, liwdg_entry_new (manager),
		&self->widgets.label_friction, liwdg_label_new (manager),
		&self->widgets.label_name, liwdg_label_new (manager),
		&self->widgets.label_model, liwdg_label_new (manager),
		&self->widgets.label_occlude, liwdg_label_new (manager),
		&self->widgets.label_shader1, liwdg_label_new (manager),
		&self->widgets.label_shader2, liwdg_label_new (manager),
		&self->widgets.label_texture1, liwdg_label_new (manager),
		&self->widgets.label_texture2, liwdg_label_new (manager),
		&self->widgets.label_type, liwdg_label_new (manager),
		&self->widgets.scroll_friction, liwdg_scroll_new (manager), NULL))
		return 0;

	/* Configure widgets. */
	liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_type), "Tile");
	liwdg_check_set_text (LIWDG_CHECK (self->widgets.check_occlude), "Enable");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_friction), "Friction:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_model), "Model:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_name), "Name:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_occlude), "Occluder:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_shader1), "Shader 1:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_shader2), "Shader 2:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_texture1), "Texture 1:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_texture2), "Texture 2:");
	liwdg_label_set_text (LIWDG_LABEL (self->widgets.label_type), "Type:");
	liwdg_widget_insert_callback (self->widgets.button_type, "pressed", private_pressed_type, self);
	liwdg_widget_insert_callback (self->widgets.check_occlude, "pressed", private_pressed_occlude, self);
	liwdg_widget_insert_callback (self->widgets.entry_model, "edited", private_changed_model, self);
	liwdg_widget_insert_callback (self->widgets.entry_name, "edited", private_changed_name, self);
	liwdg_widget_insert_callback (self->widgets.entry_shader1, "edited", private_changed_shader, self);
	liwdg_widget_insert_callback (self->widgets.entry_shader2, "edited", private_changed_shader, self);
	liwdg_widget_insert_callback (self->widgets.entry_texture1, "edited", private_changed_texture, self);
	liwdg_widget_insert_callback (self->widgets.entry_texture2, "edited", private_changed_texture, self);
	liwdg_widget_insert_callback (self->widgets.scroll_friction, "pressed", private_changed_friction, self);

	/* Pack self. */
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->widgets.label_type);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 0, self->widgets.button_type);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 1, self->widgets.label_name);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 1, self->widgets.entry_name);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 2, self->widgets.label_model);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 2, self->widgets.entry_model);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 3, self->widgets.label_shader1);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 3, self->widgets.entry_shader1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 4, self->widgets.label_shader2);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 4, self->widgets.entry_shader2);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 5, self->widgets.label_texture1);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 5, self->widgets.entry_texture1);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 6, self->widgets.label_texture2);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 6, self->widgets.entry_texture2);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 7, self->widgets.label_friction);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 7, self->widgets.scroll_friction);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 8, self->widgets.label_occlude);
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 8, self->widgets.check_occlude);
	liwdg_widget_set_visible (LIWDG_WIDGET (self), 0);

	return 1;
}

static void
private_free (LIExtMaterial* self)
{
}

static int
private_event (LIExtMaterial* self,
               LIWdgEvent*    event)
{
	return liwdg_widget_group ()->event (LIWDG_WIDGET (self), event);
}

static int
private_pressed_occlude (LIExtMaterial* self,
                         LIWdgWidget*   widget)
{
	int value;

	lisys_assert (self->material != NULL);

	value = liwdg_check_get_active (LIWDG_CHECK (self->widgets.check_occlude));
	if (!value)
		self->material->flags &= ~LIVOX_MATERIAL_FLAG_OCCLUDER;
	else
		self->material->flags |= LIVOX_MATERIAL_FLAG_OCCLUDER;
	private_rebuild_type (self);
	private_rebuild_preview (self);

	return 0;
}

static int
private_pressed_type (LIExtMaterial* self,
                      LIWdgWidget*   widget)
{
	lisys_assert (self->material != NULL);

	self->material->type = (self->material->type + 1) % LIVOX_MATERIAL_TYPE_MAX;
	private_rebuild_type (self);
	private_rebuild_preview (self);

	return 0;
}

static int
private_changed_friction (LIExtMaterial* self,
                          LIWdgWidget*   widget)
{
	lisys_assert (self->material != NULL);

	self->material->friction = liwdg_scroll_get_value (LIWDG_SCROLL (widget));

	return 0;
}

static int
private_changed_model (LIExtMaterial* self,
                       LIWdgWidget*   widget)
{
	const char* model;

	lisys_assert (self->material != NULL);

	model = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_model));
	livox_material_set_model (self->material, model);
	private_rebuild_preview (self);

	return 0;
}

static int
private_changed_name (LIExtMaterial* self,
                      LIWdgWidget*   widget)
{
	const char* name;

	lisys_assert (self->material != NULL);
	lisys_assert (self->treerow != NULL);

	name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_name));
	livox_material_set_name (self->material, name);
	liwdg_treerow_set_text (self->treerow, name);

	return 0;
}

static int
private_changed_shader (LIExtMaterial* self,
                        LIWdgWidget*   widget)
{
	const char* name;

	lisys_assert (self->material != NULL);

	name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_shader1));
	limdl_material_set_shader (&self->material->mat_top, name);
	name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_shader2));
	limdl_material_set_shader (&self->material->mat_side, name);
	private_rebuild_preview (self);

	return 0;
}

static int
private_changed_texture (LIExtMaterial* self,
                         LIWdgWidget*   widget)
{
	int i;
	int count;
	int flags;
	char** names;
	const char* name;

	lisys_assert (self->material != NULL);

	flags = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;

	/* Set top textures. */
	limdl_material_clear_textures (&self->material->mat_top);
	name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_texture1));
	if (listr_split (name, ' ', &names, &count))
	{
		for (i = 0 ; i < count ; i++)
		{
			limdl_material_append_texture (&self->material->mat_top, LIMDL_TEXTURE_TYPE_IMAGE, flags, names[i]);
			lisys_free (names[i]);
		}
		lisys_free (names);
	}

	/* Set side textures. */
	limdl_material_clear_textures (&self->material->mat_side);
	name = liwdg_entry_get_text (LIWDG_ENTRY (self->widgets.entry_texture2));
	if (listr_split (name, ' ', &names, &count))
	{
		for (i = 0 ; i < count ; i++)
		{
			limdl_material_append_texture (&self->material->mat_side, LIMDL_TEXTURE_TYPE_IMAGE, flags, names[i]);
			lisys_free (names[i]);
		}
		lisys_free (names);
	}

	private_rebuild_preview (self);

	return 0;
}

static void
private_rebuild_preview (LIExtMaterial* self)
{
	LIMatVector eye;
	LIMatVector ctr;
	LIMatVector up;

	if (self->material == NULL)
	{
		liext_preview_clear (LIEXT_PREVIEW (self->preview));
		liext_preview_build (LIEXT_PREVIEW (self->preview));
	}
	else
	{
		liext_preview_clear (LIEXT_PREVIEW (self->preview));
		liext_preview_build_tile (LIEXT_PREVIEW (self->preview), self->material->id);
		eye = limat_vector_init (0.0f, 0.0f, 0.0f);
		ctr = limat_vector_init (0.0f, 0.0f, 0.0f);
		up = limat_vector_init (0.0f, 1.0f, 0.0f);
		LIEXT_PREVIEW (self->preview)->camera->config.distance = 6.0f;
		liext_preview_setup_camera (LIEXT_PREVIEW (self->preview),
			&eye, &ctr, &up, LIALG_CAMERA_THIRDPERSON);
	}
}

static void
private_rebuild_type (LIExtMaterial* self)
{
	switch (self->material->type)
	{
		case LIVOX_MATERIAL_TYPE_CORNER:
			liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_type), "Corners");
			break;
		case LIVOX_MATERIAL_TYPE_HEIGHT:
			liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_type), "Heightmap");
			break;
		default:
			liwdg_button_set_text (LIWDG_BUTTON (self->widgets.button_type), "Tile");
			break;
	}
}

static char*
private_texture_string (LIExtMaterial* self,
                        LIMdlMaterial* material)
{
	int i;
	int len;
	int len1;
	char* tmp;
	char* name;
	const char* texture;

	len = 0;
	name = NULL;
	for (i = 0 ; i < material->textures.count ; i++)
	{
		if (material->textures.array[i].type == LIMDL_TEXTURE_TYPE_IMAGE)
		{
			texture = material->textures.array[i].string;
			len1 = strlen (texture);
			if (name != NULL)
			{
				tmp = lisys_realloc (name, len + len1 + 2);
				if (tmp != NULL)
				{
					name = tmp;
					name[len] = ' ';
					strcpy (name + len + 1, texture);
					len += len1 + 1;
				}
			}
			else
			{
				name = listr_dup (texture);
				len = len1;
			}
		}
	}

	return name;
}

/** @} */
/** @} */
/** @} */
