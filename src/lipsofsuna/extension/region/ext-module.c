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

LIMaiExtensionInfo liext_region_info =
{
	LIMAI_EXTENSION_VERSION, "Region",
	liext_regions_new,
	liext_regions_free
};

LIExtModule* liext_regions_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->sectors = program->sectors;

	/* Allocate sector data. */
	if (!lialg_sectors_insert_content (self->sectors, "region", self,
	     	(LIAlgSectorFreeFunc) liext_sector_free,
	     	(LIAlgSectorLoadFunc) liext_sector_new))
	{
		liext_regions_free (self);
		return NULL;
	}

	liscr_script_create_class (program->script, "Region", liext_script_region, self);

	return self;
}

void liext_regions_free (
	LIExtModule* self)
{
	/* Free sector data. */
	if (self->sectors != NULL)
		lialg_sectors_remove_content (self->sectors, "region");

	lisys_free (self);
}

/** @} */
/** @} */
