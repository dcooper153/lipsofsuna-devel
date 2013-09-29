/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExtSectors Sectors
 * @{
 */

#include "module.h"

static void Sectors_refresh (LIScrArgs* args)
{
	float radius;
	LIExtSectorsModule* module;
	LIMatVector position;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SECTORS);
	if (!liscr_args_geti_vector (args, 0, &position) ||
	    !liscr_args_geti_float (args, 1, &radius))
		return;
	lialg_sectors_refresh_point (module->sectors, &position, radius);
}

static void Sectors_unload_all (LIScrArgs* args)
{
	LIExtSectorsModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SECTORS);
	lialg_sectors_clear (module->sectors);
}

static void Sectors_unload_sector (LIScrArgs* args)
{
	int sector;
	LIExtSectorsModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SECTORS);
	if (liscr_args_gets_int (args, "sector", &sector))
		lialg_sectors_remove (module->sectors, sector);
}

static void Sectors_update (LIScrArgs* args)
{
	float secs;
	LIExtSectorsModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SECTORS);
	if (liscr_args_geti_float (args, 0, &secs))
		liext_sectors_update (module, secs);
}

static void Sectors_get_sectors (LIScrArgs* args)
{
	int idle;
	LIAlgSector* sector;
	LIAlgU32dicIter iter;
	LIExtSectorsModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SECTORS);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, module->sectors->sectors)
	{
		sector = iter.value;
		idle = lisys_time (NULL) - (time_t) sector->stamp;
		liscr_args_setf_float (args, iter.key, idle);
	}
}

static void Sectors_get_sector_idle (LIScrArgs* args)
{
	int id;
	LIAlgSector* sector;
	LIExtSectorsModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SECTORS);
	if (!liscr_args_geti_int (args, 0, &id))
		return;
	sector = lialg_u32dic_find (module->sectors->sectors, id);
	if (sector != NULL)
		liscr_args_seti_int (args, lisys_time (NULL) - (time_t) sector->stamp);
}

static void Sectors_get_sector_size (LIScrArgs* args)
{
	LIExtSectorsModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SECTORS);
	liscr_args_seti_float (args, module->sectors->width);
}

/*****************************************************************************/

void liext_script_sectors (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SECTORS, "sectors_refresh", Sectors_refresh);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SECTORS, "sectors_unload_all", Sectors_unload_all);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SECTORS, "sectors_unload_sector", Sectors_unload_sector);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SECTORS, "sectors_update", Sectors_update);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SECTORS, "sectors_get_sectors", Sectors_get_sectors);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SECTORS, "sectors_get_sector_idle", Sectors_get_sector_idle);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SECTORS, "sectors_get_sector_size", Sectors_get_sector_size);
}

/** @} */
/** @} */
