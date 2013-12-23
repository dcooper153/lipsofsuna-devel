/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

static void private_sector_free (
	void*        data,
	LIAlgSector* sector);

static void private_sector_load (
	void*        data,
	LIAlgSector* sector);

/*****************************************************************************/

LIMaiExtensionInfo liext_sectors_info =
{
	LIMAI_EXTENSION_VERSION, "Sectors",
	liext_sectors_new,
	liext_sectors_free,
	liext_sectors_get_memstat
};

LIExtSectorsModule* liext_sectors_new (
	LIMaiProgram* program)
{
	LIExtSectorsModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtSectorsModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Initialize sectors. */
#warning Hardcoded sector size
	self->sectors = lialg_sectors_new (128, 16.0f);
	if (self->sectors == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->sectors->sector_free_callback.callback = private_sector_free;
	self->sectors->sector_free_callback.userdata = self;
	self->sectors->sector_load_callback.callback = private_sector_load;
	self->sectors->sector_load_callback.userdata = self;

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_SECTORS, self);
	liext_script_sectors (program->script);

	return self;
}

void liext_sectors_free (
	LIExtSectorsModule* self)
{
	/* Clear all sector data. */
	if (self->sectors != NULL)
		lialg_sectors_clear (self->sectors);

	/* Free the sectors. */
	if (self->sectors != NULL)
		lialg_sectors_free (self->sectors);

	lisys_free (self);
}

void liext_sectors_update (
	LIExtSectorsModule* self,
	float               secs)
{
	lialg_sectors_update (self->sectors, secs);
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_sectors_get_memstat (
	LIExtSectorsModule* self,
	LIMaiMemstat*       stat)
{
/* TODO: memstat */
}

/*****************************************************************************/

static void private_sector_free (
	void*        data,
	LIAlgSector* sector)
{
	LIExtSectorsModule* self = data;

	/* Invoke callbacks. */
	lical_callbacks_call (self->program->callbacks, "sector-free", lical_marshal_DATA_INT, sector->index);

	/* Inform scripts. */
	limai_program_event (self->program, "sector-free", "sector", LIMAI_FIELD_INT, sector->index, NULL);
}

static void private_sector_load (
	void*        data,
	LIAlgSector* sector)
{
	LIExtSectorsModule* self = data;

	/* Invoke callbacks. */
	lical_callbacks_call (self->program->callbacks, "sector-load", lical_marshal_DATA_INT, sector->index);

	/* Inform scripts. */
	limai_program_event (self->program, "sector-load", "sector", LIMAI_FIELD_INT, sector->index, NULL);
}

/** @} */
/** @} */
