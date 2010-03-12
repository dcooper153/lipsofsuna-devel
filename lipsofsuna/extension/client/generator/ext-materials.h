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

#ifndef __EXT_MATERIALS_H__
#define __EXT_MATERIALS_H__

#include "ext-module.h"

#define LIEXT_MATERIALS(o) ((LIExtMaterials*)(o))

typedef struct _LIExtMaterialsTreerow LIExtMaterialsTreerow;
struct _LIExtMaterialsTreerow
{
	LIVoxMaterial* material;
};

struct _LIExtMaterials
{
	LIWdgGroup base;
	LIExtModule* module;
	LIGenGenerator* generator;
	struct
	{
		LIWdgWidget* group_view;
		LIWdgWidget* button_add;
		LIWdgWidget* button_remove;
		LIWdgWidget* material;
		LIWdgWidget* preview;
		LIWdgWidget* tree;
		LIWdgWidget* view;
	} widgets;
};

const LIWdgClass*
liext_widget_materials ();

LIWdgWidget*
liext_materials_new (LIWdgManager* manager,
                     LIExtModule*  module);

int
liext_materials_save (LIExtMaterials* self);

void
liext_materials_reset (LIExtMaterials* self,
                       LIArcReader*    reader);

int
liext_materials_get_active (LIExtMaterials* self);

#endif

/** @} */
/** @} */
/** @} */
