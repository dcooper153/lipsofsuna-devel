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
private_init (liextDialogBrush* self,
              liwdgManager*     manager);

static void
private_free (liextDialogBrush* self);

static int
private_event (liextDialogBrush* self,
               liwdgEvent*       event);

static int
private_cancel (liextDialogBrush* self,
                liwdgWidget*      widget);

static int
private_create (liextDialogBrush* self,
                liwdgWidget*      widget);

static int
private_update_brush (liextDialogBrush* self);

/****************************************************************************/

const liwdgClass liextDialogBrushType =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorBrushDialog", sizeof (liextDialogBrush),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_dialog_brush_new (liwdgManager* manager,
                        liextModule*  module)
{
	liextDialogBrush* data;
	liwdgWidget* self;

	/* Allocate self. */
	self = liwdg_widget_new (manager, &liextDialogBrushType);
	if (self == NULL)
		return NULL;
	data = LIEXT_DIALOG_BRUSH (self);
	data->module = module;

	/* Initialize preview. */
	data->preview = liext_preview_new (manager, module->module);
	if (data->preview == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liext_preview_replace_materials (LIEXT_PREVIEW (data->preview));
	liwdg_widget_set_request (data->preview, 320, 240);
	liwdg_group_set_child (LIWDG_GROUP (data), 0, 2, data->preview);
	data->generator = LIEXT_PREVIEW (data->preview)->generator;

	return self;
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgWindowType;
}

static int
private_init (liextDialogBrush* self,
              liwdgManager*     manager)
{
	int i;
	liwdgWidget* widgets[] =
	{
		liwdg_button_new (manager),
		liwdg_button_new (manager),
		liwdg_group_new_with_size (manager, 3, 2),
		liwdg_spin_new (manager),
		liwdg_spin_new (manager),
		liwdg_spin_new (manager),
		liwdg_spin_new (manager),
		liwdg_spin_new (manager),
		liwdg_spin_new (manager),
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 1, 4))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (liwdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	self->button_cancel = widgets[(i = 0)];
	self->button_create = widgets[++i];
	self->group_spins = widgets[++i];
	self->spin_xmin = widgets[++i];
	self->spin_ymin = widgets[++i];
	self->spin_zmin = widgets[++i];
	self->spin_xmax = widgets[++i];
	self->spin_ymax = widgets[++i];
	self->spin_zmax = widgets[++i];

	/* Pack spins. */
	liwdg_group_set_homogeneous (LIWDG_GROUP (self->group_spins), 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (self->group_spins), 0, 1);
	liwdg_group_set_child (LIWDG_GROUP (self->group_spins), 0, 1, self->spin_xmin);
	liwdg_group_set_child (LIWDG_GROUP (self->group_spins), 0, 0, self->spin_xmax);
	liwdg_group_set_child (LIWDG_GROUP (self->group_spins), 1, 1, self->spin_ymin);
	liwdg_group_set_child (LIWDG_GROUP (self->group_spins), 1, 0, self->spin_ymax);
	liwdg_group_set_child (LIWDG_GROUP (self->group_spins), 2, 1, self->spin_zmin);
	liwdg_group_set_child (LIWDG_GROUP (self->group_spins), 2, 0, self->spin_zmax);
	liwdg_widget_insert_callback (self->spin_xmin, LIWDG_CALLBACK_PRESSED, 0, private_update_brush, self, NULL);
	liwdg_widget_insert_callback (self->spin_ymin, LIWDG_CALLBACK_PRESSED, 0, private_update_brush, self, NULL);
	liwdg_widget_insert_callback (self->spin_zmin, LIWDG_CALLBACK_PRESSED, 0, private_update_brush, self, NULL);
	liwdg_widget_insert_callback (self->spin_xmax, LIWDG_CALLBACK_PRESSED, 0, private_update_brush, self, NULL);
	liwdg_widget_insert_callback (self->spin_ymax, LIWDG_CALLBACK_PRESSED, 0, private_update_brush, self, NULL);
	liwdg_widget_insert_callback (self->spin_zmax, LIWDG_CALLBACK_PRESSED, 0, private_update_brush, self, NULL);

	/* Pack self. */
	liwdg_button_set_text (LIWDG_BUTTON (self->button_cancel), "Cancel");
	liwdg_button_set_text (LIWDG_BUTTON (self->button_create), "Create");
	liwdg_widget_insert_callback (self->button_cancel, LIWDG_CALLBACK_PRESSED, 0, private_cancel, self, NULL);
	liwdg_widget_insert_callback (self->button_create, LIWDG_CALLBACK_PRESSED, 0, private_create, self, NULL);
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 0, 2);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 3, self->group_spins);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 1, self->button_create);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->button_cancel);
	liwdg_window_set_title (LIWDG_WINDOW (self), "Add Brush");

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
private_free (liextDialogBrush* self)
{
}

static int
private_event (liextDialogBrush* self,
               liwdgEvent*       event)
{
	return liwdgWindowType.event (LIWDG_WIDGET (self), event);
}

static int
private_cancel (liextDialogBrush* self,
                liwdgWidget*      widget)
{
	liwdg_widget_set_visible (LIWDG_WIDGET (self), 0);
	return 0;
}

static int
private_create (liextDialogBrush* self,
                liwdgWidget*      widget)
{
	ligenBrush* src;
	ligenBrush* dst;
	liextDialog* dialog;

	/* Check for brush. */
	src = ligen_generator_find_brush (self->generator, 0);
	if (src == NULL)
		return 0;

	/* Clone the brush. */
	dialog = LIEXT_DIALOG (self->module->dialog);
	dst = ligen_brush_new (dialog->generator, src->size[0], src->size[1], src->size[2]);
	if (dst == NULL)
		return 0;
	memcpy (dst->voxels.array, src->voxels.array, src->voxels.count * sizeof (livoxVoxel));

	/* Add to the real generator. */
	if (!ligen_generator_insert_brush (dialog->generator, dst))
	{
		ligen_brush_free (dst);
		return 0;
	}
	liwdg_widget_set_visible (LIWDG_WIDGET (self), 0);
	liext_dialog_update (dialog);

	return 0;
}

static int
private_update_brush (liextDialogBrush* self)
{
	int xmin;
	int ymin;
	int zmin;
	int xmax;
	int ymax;
	int zmax;
	ligenBrush* brush;

	/* Remove old brush. */
	liext_preview_clear (LIEXT_PREVIEW (self->preview));
	ligen_generator_remove_brush (self->generator, 0);

	/* FIXME: Gets size and offset from entry_debug. */
	xmin = (int) liwdg_spin_get_value (LIWDG_SPIN (self->spin_xmin));
	ymin = (int) liwdg_spin_get_value (LIWDG_SPIN (self->spin_ymin));
	zmin = (int) liwdg_spin_get_value (LIWDG_SPIN (self->spin_zmin));
	xmax = (int) liwdg_spin_get_value (LIWDG_SPIN (self->spin_xmax));
	ymax = (int) liwdg_spin_get_value (LIWDG_SPIN (self->spin_ymax));
	zmax = (int) liwdg_spin_get_value (LIWDG_SPIN (self->spin_zmax));
	if (xmin > xmax || ymin > ymax || zmin > zmax)
		return 0;

	/* Create new brush. */
	brush = ligen_brush_new (self->generator, xmax - xmin + 1, ymax - ymin + 1, zmax - zmin + 1);
	if (brush == NULL)
		return 0;
	if (!ligen_generator_insert_brush (self->generator, brush))
	{
		ligen_brush_free (brush);
		return 0;
	}

	/* FIXME: Copies voxels from around the player. */
	if (self->module->module->network != NULL)
	{
		liengObject* player = licli_module_get_player (self->module->module);
		if (player != NULL)
		{
			limatTransform t;
			lieng_object_get_transform (player, &t);
			xmin += (int)(t.position.x / LIVOX_TILE_WIDTH);
			ymin += (int)(t.position.y / LIVOX_TILE_WIDTH);
			zmin += (int)(t.position.z / LIVOX_TILE_WIDTH);
			livox_manager_copy_voxels (self->module->module->voxels, xmin, ymin, zmin,
				brush->size[0], brush->size[1], brush->size[2], brush->voxels.array);
		}
	}

	/* Rebuild preview. */
	liext_preview_insert_stroke (LIEXT_PREVIEW (self->preview), 0, 0, 0, 0);
	liext_preview_build (LIEXT_PREVIEW (self->preview));

	return 1;
}

/** @} */
/** @} */
/** @} */
