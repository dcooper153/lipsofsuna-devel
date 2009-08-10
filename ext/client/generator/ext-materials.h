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

#ifndef __EXT_MATERIALS_H__
#define __EXT_MATERIALS_H__

#include <client/lips-client.h>
#include <generator/lips-generator.h>
#include <engine/lips-engine.h>
#include <widget/lips-widget.h>
#include "ext-module.h"

#define LIEXT_MATERIALS(o) ((liextMaterials*)(o))

typedef struct _liextMaterialsTreerow liextMaterialsTreerow;
struct _liextMaterialsTreerow
{
	livoxMaterial* material;
	int texture;
};

struct _liextMaterials
{
	liwdgGroup base;
	liextModule* module;
	ligenGenerator* generator;
	struct
	{
		liwdgWidget* group_name;
		liwdgWidget* group_scale;
		liwdgWidget* group_view;
		liwdgWidget* button_add;
		liwdgWidget* button_remove;
		liwdgWidget* entry_name;
		liwdgWidget* label_type;
		liwdgWidget* label_scale;
		liwdgWidget* preview;
		liwdgWidget* scroll_scale;
		liwdgWidget* tree;
	} widgets;
};

extern const liwdgClass liextMaterialsType;

liwdgWidget*
liext_materials_new (liwdgManager* manager,
                     liextModule*  module);

int
liext_materials_save (liextMaterials* self);

void
liext_materials_reset (liextMaterials* self);

void
liext_materials_update (liextMaterials* self);

#endif

/** @} */
/** @} */
/** @} */
