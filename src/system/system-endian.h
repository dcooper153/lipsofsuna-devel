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
 * \addtogroup lisys System
 * @{
 * \addtogroup lisysEndian Endian
 * @{
 */

#ifndef __SYSTEM_ENDIAN_H__
#define __SYSTEM_ENDIAN_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined HAVE_ENDIAN_H

#include <endian.h>
#define LI_BYTE_ORDER __BYTE_ORDER
#define LI_LITTLE_ENDIAN __LITTLE_ENDIAN
#define LI_BIG_ENDIAN __BIG_ENDIAN

#elif defined HAVE_RPCNDR_H

#define LI_BIG_ENDIAN 4321
#define LI_LITTLE_ENDIAN 1234
#define LI_BYTE_ORDER LI_LITTLE_ENDIAN
/*
#include <windows.h>
#include <rpcndr.h>
#define LI_LITTLE_ENDIAN NDR_LITTLE_ENDIAN
#define LI_BIG_ENDIAN NDR_BIG_ENDIAN
#define LI_BYTE_ORDER NDR_LOCAL_ENDIAN
*/

#elif defined HAVE_SYS_BYTEORDER_H

#include <sys/byteorder.h>
#define LI_BIG_ENDIAN 4321
#define LI_LITTLE_ENDIAN 1234
#ifdef _BIG_ENDIAN
#define LI_BYTE_ORDER LI_BIG_ENDIAN
#else
#define LI_BYTE_ORDER LI_LITTLE_ENDIAN
#endif

#else

#error "Cannot determine byte order"

#endif

#endif

/** @} */
/** @} */

