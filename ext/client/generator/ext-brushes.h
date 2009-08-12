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

#ifndef __EXT_BRUSHES_H__
#define __EXT_BRUSHES_H__

#include <client/lips-client.h>
#include <generator/lips-generator.h>
#include <engine/lips-engine.h>
#include <widget/lips-widget.h>
#include "ext-materials.h"
#include "ext-module.h"

#define LIEXT_BRUSHES(o) ((liextBrushes*)(o))

struct _liextBrushes
{
	liwdgGroup base;
	liextModule* module;
	ligenGenerator* generator;
	struct
	{
		liwdgWidget* dialog;
		liwdgWidget* group_view;
		liwdgWidget* button_move_left;
		liwdgWidget* button_move_right;
		liwdgWidget* button_move_up;
		liwdgWidget* button_move_down;
		liwdgWidget* button_add;
		liwdgWidget* button_remove;
		liwdgWidget* entry_name;
		liwdgWidget* label_type;
		liwdgWidget* preview;
		liwdgWidget* tree;
	} widgets;
};

extern const liwdgClass liextBrushesType;

liwdgWidget*
liext_brushes_new (liwdgManager* manager,
                   liextModule*  module);

int
liext_brushes_insert_brush (liextBrushes* self,
                            ligenBrush*   brush);

int
liext_brushes_save (liextBrushes* self);

void
liext_brushes_reset (liextBrushes* self);

#endif

/** @} */
/** @} */
/** @} */
