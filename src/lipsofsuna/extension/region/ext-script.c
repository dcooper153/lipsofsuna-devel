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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtRegion Region
 * @{
 */

#include "ext-module.h"
#include "ext-sector.h"

/* @luadoc
 * module "Extension.Server.Region"
 * --- Manage map regions.
 * -- @name Region
 * -- @class table
 */

/* @luadoc
 * --- Finds the regions intersecting with a point.
 * --
 * -- @param self Region class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Position vector. (required)</li></ul>
 * -- @return Table of regions.
 * Region.find(self, args)
 */
static void Region_find (LIScrArgs* args)
{
	LIAlgSectorsIter iter;
	LIAlgSectors* sectors;
	LIExtModule* module;
	LIExtRegion* region;
	LIExtSector* sector;
	LIMatVector point;
	LIScrData* data;
	LIScrData* data1;

	liscr_script_set_gc (args->script, 0);
	if (liscr_args_gets_vector (args, "point", &point))
	{
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_REGION);
		sectors = module->program->sectors;
		LIALG_SECTORS_FOREACH_POINT (iter, sectors, &point, sectors->width)
		{
			sector = lialg_sectors_data_index (sectors, "region", iter.sector->index, 0);
			if (sector == NULL)
				continue;
			for (region = sector->regions ; region != NULL ; region = region->next)
			{
				if (point.x < region->position.x || region->position.x + region->size.x < point.x ||
				    point.y < region->position.y || region->position.y + region->size.y < point.y ||
				    point.z < region->position.z || region->position.z + region->size.z < point.z)
					continue;
				data = liscr_data_new_alloc (args->script, sizeof (void*), args->clss);
				if (data == NULL)
					continue;
				liscr_pushdata (args->lua, data);
				lua_pushnumber (args->lua, region->type);
				lua_setfield (args->lua, -2, "type");
				lua_pushnumber (args->lua, region->flags);
				lua_setfield (args->lua, -2, "flags");
				lua_pushnumber (args->lua, region->value);
				lua_setfield (args->lua, -2, "value");
				data1 = liscr_vector_new (args->script, &region->position);
				if (data1 != NULL)
				{
					liscr_pushdata (args->lua, data1);
					lua_setfield (args->lua, -2, "position");
				}
				data1 = liscr_vector_new (args->script, &region->size);
				if (data1 != NULL)
				{
					liscr_pushdata (args->lua, data1);
					lua_setfield (args->lua, -2, "size");
				}
				lua_pop (args->lua, 1);
				liscr_args_seti_data (args, data);
			}
		}
	}
	liscr_script_set_gc (args->script, 1);
}

/*****************************************************************************/

void
liext_script_region (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_REGION, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "find", Region_find);
}

/** @} */
/** @} */
