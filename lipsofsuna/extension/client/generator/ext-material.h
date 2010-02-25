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

#ifndef __EXT_MATERIAL_H__
#define __EXT_MATERIAL_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/generator.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/widget.h>
#include "ext-module.h"
#include "ext-preview.h"

#define LIEXT_MATERIAL(o) ((LIExtMaterial*)(o))

struct _LIExtMaterial
{
	LIWdgGroup base;
	LIExtModule* module;
	LIExtPreview* preview;
	LIVoxMaterial* material;
	LIWdgTreerow* treerow;
	struct
	{
		LIWdgWidget* button_type;
		LIWdgWidget* check_occlude;
		LIWdgWidget* entry_name;
		LIWdgWidget* entry_model;
		LIWdgWidget* entry_shader1;
		LIWdgWidget* entry_shader2;
		LIWdgWidget* entry_texture1;
		LIWdgWidget* entry_texture2;
		LIWdgWidget* label_friction;
		LIWdgWidget* label_name;
		LIWdgWidget* label_model;
		LIWdgWidget* label_occlude;
		LIWdgWidget* label_shader1;
		LIWdgWidget* label_shader2;
		LIWdgWidget* label_texture1;
		LIWdgWidget* label_texture2;
		LIWdgWidget* label_type;
		LIWdgWidget* scroll_friction;
	} widgets;
};

extern const LIWdgClass liext_widget_material;

LIWdgWidget*
liext_material_new (LIWdgManager* manager,
                    LIExtModule*  module,
                    LIExtPreview* preview);

void
liext_material_set_material (LIExtMaterial* self,
                             LIVoxMaterial* material,
                             LIWdgTreerow*  treerow);

#endif

/** @} */
/** @} */
/** @} */
