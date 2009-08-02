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
#include <generator/lips-generator.h>
#include <engine/lips-engine.h>
#include <widget/lips-widget.h>
#include "ext-module.h"

#define LIEXT_DIALOG(o) ((liextDialog*)(o))

struct _liextDialog
{
	liwdgWindow base;
	liextModule* module;
	ligenGenerator* generator;
	int active_brush;
	int active_rule;
	int active_stroke;
	float timer;
	struct
	{
		int count;
		int* array;
	} brushes;
	struct
	{
		liwdgWidget* dialog;
		liwdgWidget* group_column;
		liwdgWidget* group_brushes;
		liwdgWidget* group_rules;
		liwdgWidget* group_strokes;
		liwdgWidget* button_move_left;
		liwdgWidget* button_move_right;
		liwdgWidget* button_move_up;
		liwdgWidget* button_move_down;
		liwdgWidget* button_add_brush;
		liwdgWidget* button_remove_brush;
		liwdgWidget* button_add_rule;
		liwdgWidget* button_remove_rule;
		liwdgWidget* button_add_stroke;
		liwdgWidget* button_edit_stroke;
		liwdgWidget* button_remove_stroke;
		liwdgWidget* preview;
	} widgets;
};

extern const liwdgClass liextDialogType;

liwdgWidget*
liext_dialog_new (liwdgManager* manager,
                  liextModule*  module);

int
liext_dialog_save (liextDialog* self);

void
liext_dialog_update (liextDialog* self);

#endif

/** @} */
/** @} */
/** @} */
