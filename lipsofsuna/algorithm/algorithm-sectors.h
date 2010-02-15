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
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup LIAlgSectors Sectors
 * @{
 */

#ifndef __ALGORITHM_SECTORS_H__
#define __ALGORITHM_SECTORS_H__

#include <lipsofsuna/math.h>
#include "algorithm-strdic.h"
#include "algorithm-u32dic.h"

typedef struct _LIAlgSector LIAlgSector;
typedef struct _LIAlgSectors LIAlgSectors;
typedef struct _LIAlgSectorsCallback LIAlgSectorsCallback;
typedef void (*LIAlgSectorFreeFunc)(void*);
typedef void* (*LIAlgSectorLoadFunc)(LIAlgSector*);

struct _LIAlgSectorsCallback
{
	void (*callback)(void*, LIAlgSector*);
	void* userdata;
};

struct _LIAlgSector
{
	int index;
	int stamp;
	int x;
	int y;
	int z;
	LIMatVector position;
	LIAlgSectors* manager;
	LIAlgStrdic* content;
};

struct _LIAlgSectors
{
	int count;
	int loading;
	float width;
	LIAlgU32dic* sectors;
	LIAlgStrdic* content;
	LIAlgSectorsCallback sector_free_callback;
	LIAlgSectorsCallback sector_load_callback;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAlgSectors*
lialg_sectors_new (int   count,
                   float width);

void
lialg_sectors_free (LIAlgSectors* self);

void
lialg_sectors_clear (LIAlgSectors* self);

void*
lialg_sectors_data_index (LIAlgSectors* self,
                          const char*   name,
                          int           index,
                          int           create);

void*
lialg_sectors_data_offset (LIAlgSectors* self,
                           const char*   name,
                           int           x,
                           int           y,
                           int           z,
                           int           create);

void*
lialg_sectors_data_point (LIAlgSectors*      self,
                          const char*        name,
                          const LIMatVector* point,
                          int                create);

LIAlgSector*
lialg_sectors_sector_index (LIAlgSectors* self,
                            int           index,
                            int           create);

LIAlgSector*
lialg_sectors_sector_offset (LIAlgSectors* self,
                             int           x,
                             int           y,
                             int           z,
                             int           create);

LIAlgSector*
lialg_sectors_sector_point (LIAlgSectors*      self,
                            const LIMatVector* point,
                            int                create);

void
lialg_sectors_index_to_offset (LIAlgSectors* self,
                               int           index,
                               int*          x,
                               int*          y,
                               int*          z);

void
lialg_sectors_index_to_offset_static (int  count,
                                      int  index,
                                      int* x,
                                      int* y,
                                      int* z);

int
lialg_sectors_insert_content (LIAlgSectors*       self,
                              const char*         name,
                              void*               data,
                              LIAlgSectorFreeFunc free,
                              LIAlgSectorLoadFunc load);

int
lialg_sectors_offset_to_index (LIAlgSectors* self,
                               int           x,
                               int           y,
                               int           z);

int
lialg_sectors_offset_to_index_static (int count,
                                      int x,
                                      int y,
                                      int z);

int
lialg_sectors_point_to_index (LIAlgSectors*      self,
                              const LIMatVector* point);

int
lialg_sectors_point_to_index_static (int                count,
                                     float              width,
                                     const LIMatVector* point);

void
lialg_sectors_refresh_point (LIAlgSectors*      self,
                             const LIMatVector* point,
                             float              radius);

void
lialg_sectors_remove (LIAlgSectors* self,
                      int           index);

void
lialg_sectors_remove_content (LIAlgSectors* self,
                              const char*   name);

void*
lialg_sectors_get_userdata (LIAlgSectors* self,
                            const char*   name);

#ifdef __cplusplus
}
#endif

#endif
