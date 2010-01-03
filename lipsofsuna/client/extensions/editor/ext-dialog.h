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

#ifndef __EXT_DIALOG_H__
#define __EXT_DIALOG_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/widget.h>
#include "ext-editor.h"

#define LIEXT_DIALOG(o) ((LIExtDialog*)(o))

typedef struct _LIExtDialog LIExtDialog;
struct _LIExtDialog
{
	LIWdgWindow base;
	LICliClient* client;
	LIExtEditor* editor;
	LIWdgWidget* group;
	LIWdgWidget* label_model;
	LIWdgWidget* entry_model;
	LIWdgWidget* button_edit;
	LIWdgWidget* button_insert;
	LIWdgWidget* button_delete;
	LIWdgWidget* button_duplicate;
};

extern const LIWdgClass liext_widget_dialog;

LIWdgWidget*
liext_dialog_new (LIWdgManager* manager,
                  LIExtEditor*  editor);

LIEngModel*
liext_dialog_get_model (LIExtDialog* self);

#endif

/** @} */
/** @} */
/** @} */
