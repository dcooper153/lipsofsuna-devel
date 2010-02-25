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

#ifndef __EXT_BRUSHES_H__
#define __EXT_BRUSHES_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/generator.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/widget.h>
#include "ext-materials.h"
#include "ext-module.h"

#define LIEXT_BRUSHES(o) ((LIExtBrushes*)(o))

struct _LIExtBrushes
{
	LIWdgGroup base;
	LIExtModule* module;
	LIGenGenerator* generator;
	LIGenBrush* clipboard;
	LIMatTransform transform;
	int paint;
	struct
	{
		int index;
		int start[3];
		int end[3];
	} select;
	struct
	{
		LIWdgWidget* check_required;
		LIWdgWidget* group_paint;
		LIWdgWidget* group_view;
		LIWdgWidget* group_size;
		LIWdgWidget* button_add;
		LIWdgWidget* button_copy;
		LIWdgWidget* button_paint;
		LIWdgWidget* button_remove;
		LIWdgWidget* entry_name;
		LIWdgWidget* entry_objextra;
		LIWdgWidget* entry_objtype;
		LIWdgWidget* label_size;
		LIWdgWidget* label_type;
		LIWdgWidget* label_objprob;
		LIWdgWidget* label_objtype;
		LIWdgWidget* label_objextra;
		LIWdgWidget* preview;
		LIWdgWidget* scroll_objprob;
		LIWdgWidget* spin_axis;
		LIWdgWidget* spin_sizex;
		LIWdgWidget* spin_sizey;
		LIWdgWidget* spin_sizez;
		LIWdgWidget* tree;
		LIWdgWidget* view;
	} widgets;
};

extern const LIWdgClass liext_widget_brushes;

LIWdgWidget*
liext_brushes_new (LIWdgManager* manager,
                   LIExtModule*  module);

int
liext_brushes_insert_brush (LIExtBrushes* self,
                            LIGenBrush*   brush);

int
liext_brushes_save (LIExtBrushes* self);

void
liext_brushes_reset (LIExtBrushes* self,
                     LIArcReader*  reader);

#endif

/** @} */
/** @} */
/** @} */
