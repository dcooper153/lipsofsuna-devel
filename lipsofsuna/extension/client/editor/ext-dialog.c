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
 * \addtogroup liextcliEditor Editor
 * @{
 */

#include <lipsofsuna/system.h>
#include "ext-dialog.h"

static const void*
private_base ();

static int
private_init (LIExtDialog*    self,
              LIWdgManager* manager);

static void
private_free (LIExtDialog* self);

static int
private_delete (LIExtDialog* self);

static int
private_duplicate (LIExtDialog* self);

static int
private_edit (LIExtDialog* self);

static int
private_event (LIExtDialog* self,
               LIWdgEvent*  event);

static int
private_insert (LIExtDialog* self);

/****************************************************************************/

const LIWdgClass liext_widget_dialog =
{
	LIWDG_BASE_DYNAMIC, private_base, "EditorDialog", sizeof (LIExtDialog),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event
};

LIWdgWidget*
liext_dialog_new (LIWdgManager* manager,
                  LIExtEditor*  editor)
{
	LIWdgWidget* self;

	self = liwdg_widget_new (manager, &liext_widget_dialog);
	if (self == NULL)
		return NULL;
	LIEXT_DIALOG (self)->editor = editor;
	LIEXT_DIALOG (self)->client = editor->client;

	return self;
}

LIEngModel*
liext_dialog_get_model (LIExtDialog* self)
{
	const char* str;
	LIEngModel* model;

	str = liwdg_entry_get_text (LIWDG_ENTRY (self->entry_model));
	model = lieng_engine_find_model_by_name (self->client->engine, str);

	return model;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdg_widget_window;
}

static int
private_init (LIExtDialog*   self,
              LIWdgManager* manager)
{
	int i;
	LIWdgWidget* widgets[] =
	{
		liwdg_group_new_with_size (manager, 2, 1),
		liwdg_label_new (manager),
		liwdg_entry_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_button_new (manager),
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 1, 5))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (LIWdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	self->group = widgets[(i = 0)];
	self->label_model = widgets[++i];
	self->entry_model = widgets[++i];
	self->button_insert = widgets[++i];
	self->button_edit = widgets[++i];
	self->button_delete = widgets[++i];
	self->button_duplicate = widgets[++i];

	/* Pack entry group. */
	liwdg_label_set_text (LIWDG_LABEL (self->label_model), "Model:");
	liwdg_entry_set_text (LIWDG_ENTRY (self->entry_model), "aer-000");
	liwdg_group_set_margins (LIWDG_GROUP (self->group), 5, 0, 0, 0);
	liwdg_group_set_spacings (LIWDG_GROUP (self->group), 5, 5);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->group), 1, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 0, 0, self->label_model);
	liwdg_group_set_child (LIWDG_GROUP (self->group), 1, 0, self->entry_model);

	/* Configure buttons. */
	liwdg_button_set_text (LIWDG_BUTTON (self->button_edit), "Edit");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_insert), "Insert");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_delete), "Delete");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_duplicate), "Duplicate");
	liwdg_widget_insert_callback (self->button_edit, "pressed", private_edit, self);
	liwdg_widget_insert_callback (self->button_insert, "pressed", private_insert, self);
	liwdg_widget_insert_callback (self->button_delete, "pressed", private_delete, self);
	liwdg_widget_insert_callback (self->button_duplicate, "pressed", private_duplicate, self);

	/* Pack self. */
	liwdg_window_set_title (LIWDG_WINDOW (self), "Model");
	liwdg_group_set_margins (LIWDG_GROUP (self), 5, 5, 5, 5);
	liwdg_group_set_spacings (LIWDG_GROUP (self), 5, 5);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 4, self->group);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 3, self->button_insert);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 2, self->button_duplicate);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 1, self->button_edit);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->button_delete);

	return 1;

error:
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (LIWdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			liwdg_widget_free (widgets[i]);
	}
	return 0;
}

static void
private_free (LIExtDialog* self)
{
}

static int
private_delete (LIExtDialog* self)
{
	liext_editor_destroy (self->editor);
	return 0;
}

static int
private_duplicate (LIExtDialog* self)
{
	liext_editor_duplicate (self->editor);
	return 0;
}

static int
private_edit (LIExtDialog* self)
{
	char* name;
	const char* tmp;

	/* Get source file name. */
	tmp = liwdg_entry_get_text (LIWDG_ENTRY (self->entry_model));
	name = lisys_path_format (self->client->path,
		LISYS_PATH_SEPARATOR, "graphics",
		LISYS_PATH_SEPARATOR, tmp, ".blend", NULL);
	if (name == NULL)
		return 0;

	/* Execute Blender. */
	lisys_execvl_detach ("blender", "blender", name, NULL);
	lisys_free (name);

	return 0;
}

static int
private_event (LIExtDialog* self,
               LIWdgEvent*  event)
{
	return liwdg_widget_window.event (LIWDG_WIDGET (self), event);
}

static int
private_insert (LIExtDialog* self)
{
	LIEngModel* model;
	LIMatVector position;
	LIMatQuaternion rotation;
	LIMatTransform transform;

	model = liext_dialog_get_model (self);
	if (model == NULL)
		return 0;
	lialg_camera_get_transform (self->client->camera, &transform);
	position = limat_vector_init (0.0f, 3.0f, -15.0f);
	position = limat_transform_transform (transform, position);
	rotation = limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f);
	transform = limat_transform_init (position, rotation);
	liext_editor_create (self->editor, model->id, &transform);

	return 0;
}

/** @} */
/** @} */
/** @} */
