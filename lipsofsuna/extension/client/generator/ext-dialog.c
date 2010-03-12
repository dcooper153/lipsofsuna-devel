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
#include "ext-brushes.h"
#include "ext-dialog.h"
#include "ext-materials.h"
#include "ext-preview.h"

static int
private_init (LIExtEditor*    self,
              LIWdgManager* manager);

static void
private_free (LIExtEditor* self);

static int
private_event (LIExtEditor* self,
               LIWdgEvent*  event);

/****************************************************************************/

const LIWdgClass*
liext_widget_editor ()
{
	static LIWdgClass clss =
	{
		NULL, "GeneratorDialog", sizeof (LIExtEditor),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	clss.base = liwdg_widget_group;
	return &clss;
}

LIWdgWidget*
liext_editor_new (LIWdgManager* manager,
                  LIExtModule*  module)
{
	LIExtEditor* data;
	LIWdgWidget* self;

	/* Allocate self. */
	self = liwdg_widget_new (manager, liext_widget_editor ());
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
	data->generator = LIEXT_MATERIALS (data->materials)->generator;

	return self;
}

int
liext_editor_save (LIExtEditor* self)
{
	return liext_brushes_save (LIEXT_BRUSHES (self->brushes)) &&
	       liext_materials_save (LIEXT_MATERIALS (self->materials));
}

void
liext_editor_reset (LIExtEditor* self,
                    LIArcReader* reader)
{
	liext_brushes_reset (LIEXT_BRUSHES (self->brushes), reader);
	liext_materials_reset (LIEXT_MATERIALS (self->materials), reader);
}

/****************************************************************************/

static int
private_init (LIExtEditor*  self,
              LIWdgManager* manager)
{
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 2, 1))
		return 0;

	liwdg_group_set_spacings (LIWDG_GROUP (self), 0, 0);
	liwdg_group_set_col_expand (LIWDG_GROUP (self), 0, 1);
	liwdg_group_set_row_expand (LIWDG_GROUP (self), 0, 1);

	return 1;
}

static void
private_free (LIExtEditor* self)
{
}

static int
private_event (LIExtEditor* self,
               LIWdgEvent*  event)
{
	return liwdg_widget_group ()->event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */
/** @} */
