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
#include "ext-dialog.h"
#include "ext-materials.h"
#include "ext-preview.h"

static const void*
private_base ();

static int
private_init (liextEditor*    self,
              liwdgManager* manager);

static void
private_free (liextEditor* self);

static int
private_event (liextEditor* self,
               liwdgEvent*  event);

/****************************************************************************/

const liwdgClass liextEditorType =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorDialog", sizeof (liextEditor),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_editor_new (liwdgManager* manager,
                  liextModule*  module)
{
	liextEditor* data;
	liwdgWidget* self;

	/* Allocate self. */
	self = liwdg_widget_new (manager, &liextEditorType);
	if (self == NULL)
		return NULL;
	data = LIEXT_EDITOR (self);
	data->module = module;

	/* Initialize brush editor. */
	data->brushes = liext_brushes_new (manager, module);
	if (data->brushes == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, data->brushes);

	/* Initialize material editor. */
	data->materials = liext_materials_new (manager, module);
	if (data->materials == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	liwdg_group_set_child (LIWDG_GROUP (self), 1, 0, data->materials);

	return self;
}

int
liext_editor_save (liextEditor* self)
{
	return liext_brushes_save (LIEXT_BRUSHES (self->brushes)) &&
	       liext_materials_save (LIEXT_MATERIALS (self->materials));
}

void
liext_editor_reset (liextEditor* self,
                    liarcReader* reader)
{
	liext_brushes_reset (LIEXT_BRUSHES (self->brushes), reader);
	liext_materials_reset (LIEXT_MATERIALS (self->materials), reader);
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgGroupType;
}

static int
private_init (liextEditor*  self,
              liwdgManager* manager)
{
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 2, 1))
		return 0;

	liwdg_group_set_spacings (LIWDG_GROUP (self), 0, 0);
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 0, 1);
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 1, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self), 0, 1);

	return 1;
}

static void
private_free (liextEditor* self)
{
}

static int
private_event (liextEditor* self,
               liwdgEvent*  event)
{
	return liwdgGroupType.event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */
/** @} */
