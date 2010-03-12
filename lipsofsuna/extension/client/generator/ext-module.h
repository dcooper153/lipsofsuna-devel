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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/generator.h>
#include <lipsofsuna/widget.h>
#include <lipsofsuna/extension.h>

#define LIEXT_SCRIPT_GENERATOR "Generator"

typedef struct _LIExtBrushes LIExtBrushes;
typedef struct _LIExtEditor LIExtEditor;
typedef struct _LIExtMaterial LIExtMaterial;
typedef struct _LIExtMaterials LIExtMaterials;
typedef struct _LIExtModule LIExtModule;
typedef struct _LIExtPreview LIExtPreview;

struct _LIExtModule
{
	LICliClient* client;
	LIScrData* script;
	LIWdgWidget* editor;
};

LIExtModule*
liext_module_new (LIMaiProgram* program);

void
liext_module_free (LIExtModule* self);

int
liext_module_save (LIExtModule* self);

/*****************************************************************************/

void
liext_script_generator (LIScrClass* self,
                        void*       data);

#endif

/** @} */
/** @} */
/** @} */
