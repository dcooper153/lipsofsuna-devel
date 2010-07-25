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

#ifndef __EXT_SECTOR_H__
#define __EXT_SECTOR_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/server.h>
#include <lipsofsuna/system.h>

struct _LIExtRegion
{
	int id;
	int type;
	int flags;
	int value;
	LIExtRegion* prev;
	LIExtRegion* next;
	LIMatVector position;
	LIMatVector size;
};

struct _LIExtSector
{
	LIAlgSector* sector;
	LIExtRegion* regions;
	LISerServer* server;
};

LIExtSector* liext_sector_new (
	LIAlgSector* sector);

void liext_sector_free (
	LIExtSector* self);

int liext_sector_read (
	LIExtSector* self,
	LIArcSql*    sql);

#endif

/** @} */
/** @} */
