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
 * \addtogroup lisys System
 * @{
 * \addtogroup LISysError Error
 * @{
 */

#ifndef __SYSTEM_ERROR_H__
#define __SYSTEM_ERROR_H__

#include <errno.h>
#include "system-compiler.h"

enum
{
	LI_ERROR_NONE    =  0,
	LI_ERROR_UNKNOWN = -1,
	LI_ERROR_VERSION = -2,
};

#ifndef ENOTSUP
#define ENOTSUP 1024
#endif

#ifdef __cplusplus
extern "C" {
#endif

void
lisys_error_append (const char* format,
                                ...);

int
lisys_error_peek ();

void
lisys_error_report ();

int
lisys_error_get (const char** text);

const char*
lisys_error_get_string ();

void
lisys_error_set (int         type,
                 const char* format,
                             ...);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
