/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlIpo Ipo
 * @{
 */

#ifndef __MODEL_IPO_H__
#define __MODEL_IPO_H__

#include <archive/lips-archive.h>
#include <math/lips-math.h>

enum
{
	LIMDL_IPO_TYPE_CONSTANT,
	LIMDL_IPO_TYPE_LINEAR,
	LIMDL_IPO_TYPE_BEZIER,
};

enum
{
	LIMDL_IPO_CHANNEL_LOCX,
	LIMDL_IPO_CHANNEL_LOCY,
	LIMDL_IPO_CHANNEL_LOCZ,
	LIMDL_IPO_CHANNEL_QUATX,
	LIMDL_IPO_CHANNEL_QUATY,
	LIMDL_IPO_CHANNEL_QUATZ,
	LIMDL_IPO_CHANNEL_QUATW,
	LIMDL_IPO_CHANNEL_NUM
};

typedef struct _limdlIpoChannel limdlIpoChannel;
struct _limdlIpoChannel
{
	int type;
	int length;
	float* nodes;
};

float
limdl_ipo_channel_get_duration (const limdlIpoChannel* self);

float
limdl_ipo_channel_get_value (const limdlIpoChannel* self,
                             float                  time);

typedef struct _limdlIpo limdlIpo;
struct _limdlIpo
{
	char* name;
	limdlIpoChannel channels[LIMDL_IPO_CHANNEL_NUM];
};

int
limdl_ipo_read (limdlIpo*    self,
                liarcReader* reader);

int
limdl_ipo_write (const limdlIpo* self,
                 liarcWriter*    writer);

float
limdl_ipo_get_duration (const limdlIpo* self);

void
limdl_ipo_get_location (const limdlIpo* self,
                        float           time,
                        limatVector*    vector);

void
limdl_ipo_get_quaternion (const limdlIpo*  self,
                          float            time,
                          limatQuaternion* quaternion);

#endif

/** @} */
/** @} */
