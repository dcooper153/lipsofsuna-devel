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

#ifndef __EXT_EDITOR_H__
#define __EXT_EDITOR_H__

#include "ext-module.h"
#include "ext-materials.h"

#define LIEXT_EDITOR(o) ((LIExtEditor*)(o))

typedef struct _LIExtEditorTreerow LIExtEditorTreerow;
struct _LIExtEditorTreerow
{
	LIGenBrush* brush;
	LIGenRule* rule;
	int stroke;
};

struct _LIExtEditor
{
	LIWdgGroup base;
	LIExtModule* module;
	LIGenGenerator* generator;
	LIWdgWidget* brushes;
	LIWdgWidget* materials;
};

const LIWdgClass*
liext_widget_editor ();

LIWdgWidget*
liext_editor_new (LIWdgManager* manager,
                  LIExtModule*  module);

int
liext_editor_save (LIExtEditor* self);

void
liext_editor_reset (LIExtEditor* self,
                    LIArcReader* reader);

#endif

/** @} */
/** @} */
/** @} */
