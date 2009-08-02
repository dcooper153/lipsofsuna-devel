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

#ifndef __EXT_DIALOG_BRUSH_H__
#define __EXT_DIALOG_BRUSH_H__

#include <client/lips-client.h>
#include <generator/lips-generator.h>
#include <engine/lips-engine.h>
#include <widget/lips-widget.h>
#include "ext-module.h"

#define LIEXT_DIALOG_BRUSH(o) ((liextDialogBrush*)(o))

struct _liextDialogBrush
{
	liwdgWindow base;
	liextModule* module;
	ligenGenerator* generator;
	liwdgWidget* button_cancel;
	liwdgWidget* button_create;
	liwdgWidget* group_spins;
	liwdgWidget* preview;
	liwdgWidget* spin_xmin;
	liwdgWidget* spin_ymin;
	liwdgWidget* spin_zmin;
	liwdgWidget* spin_xmax;
	liwdgWidget* spin_ymax;
	liwdgWidget* spin_zmax;
};

extern const liwdgClass liextDialogBrushType;

liwdgWidget*
liext_dialog_brush_new (liwdgManager* manager,
                        liextModule*  module);

#endif

/** @} */
/** @} */
/** @} */
