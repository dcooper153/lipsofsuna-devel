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

#include <client/lips-client.h>
#include <engine/lips-engine.h>
#include <widget/lips-widget.h>
#include "ext-editor.h"

#define LIEXT_DIALOG(o) ((liextDialog*)(o))

typedef struct _liextDialog liextDialog;
struct _liextDialog
{
	liwdgWindow base;
	licliClient* client;
	liextEditor* editor;
	liwdgWidget* group;
	liwdgWidget* label_model;
	liwdgWidget* entry_model;
	liwdgWidget* button_edit;
	liwdgWidget* button_insert;
	liwdgWidget* button_delete;
	liwdgWidget* button_duplicate;
};

extern const liwdgClass liextDialogType;

liwdgWidget*
liext_dialog_new (liwdgManager* manager,
                  liextEditor*  editor);

liengModel*
liext_dialog_get_model (liextDialog* self);

#endif

/** @} */
/** @} */
/** @} */
