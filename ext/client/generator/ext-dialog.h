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

#ifndef __EXT_DIALOG_H__
#define __EXT_DIALOG_H__

#include <client/lips-client.h>
#include <engine/lips-engine.h>
#include <widget/lips-widget.h>
#include "ext-generator.h"

#define LIEXT_DIALOG(o) ((liextDialog*)(o))

typedef struct _liextDialog liextDialog;
struct _liextDialog
{
	liwdgWindow base;
	licliModule* module;
	liextGenerator* generator;
	liwdgWidget* group_brushes;
	liwdgWidget* group_strokes;
	liwdgWidget* label_brush;
	liwdgWidget* render_strokes;
	liwdgWidget* button_move_left;
	liwdgWidget* button_move_right;
	liwdgWidget* button_move_up;
	liwdgWidget* button_move_down;

	liwdgWidget* label_rule;
	liwdgWidget* button_insert;
	liwdgWidget* button_remove;
	liwdgWidget* button_generate;
	int active_brush;
	int active_rule;
	int active_stroke;
	float timer;
};

extern const liwdgClass liextDialogType;

liwdgWidget*
liext_dialog_new (liwdgManager*   manager,
                  liextGenerator* generator);

#endif

/** @} */
/** @} */
/** @} */
