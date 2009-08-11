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
#include "ext-materials.h"
#include "ext-module.h"

#define LIEXT_DIALOG(o) ((liextDialog*)(o))

typedef struct _liextDialogTreerow liextDialogTreerow;
struct _liextDialogTreerow
{
	ligenBrush* brush;
	ligenRule* rule;
	int stroke;
};

struct _liextDialog
{
	liwdgWindow base;
	liextModule* module;
	ligenGenerator* generator;
	liwdgWidget* brushes;
	liwdgWidget* materials;
};

extern const liwdgClass liextDialogType;

liwdgWidget*
liext_dialog_new (liwdgManager* manager,
                  liextModule*  module);

int
liext_dialog_save (liextDialog* self);

void
liext_dialog_reset (liextDialog* self);

#endif

/** @} */
/** @} */
/** @} */
