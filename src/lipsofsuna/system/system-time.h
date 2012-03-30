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

#ifndef __SYSTEM_TIME_H__
#define __SYSTEM_TIME_H__

#include "system-compiler.h"
#include <time.h>
#include <sys/time.h>

typedef struct timeval LISysTimeval;

LIAPICALL (time_t, lisys_time, (
	time_t* t));

LIAPICALL (LISysTimeval, lisys_timeval_init, ());

LIAPICALL (float, lisys_timeval_get_diff, (
	LISysTimeval start,
	LISysTimeval end));

#endif
