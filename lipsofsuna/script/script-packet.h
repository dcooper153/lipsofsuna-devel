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
 * \addtogroup liscr Script
 * @{
 * \addtogroup LIScrPacket Packet
 * @{
 */

#ifndef __SCRIPT_PACKET_H__
#define __SCRIPT_PACKET_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/string.h>

enum
{
	LISCR_PACKET_FORMAT_BOOL,
	LISCR_PACKET_FORMAT_FLOAT,
	LISCR_PACKET_FORMAT_INT8,
	LISCR_PACKET_FORMAT_INT16,
	LISCR_PACKET_FORMAT_INT32,
	LISCR_PACKET_FORMAT_STRING,
	LISCR_PACKET_FORMAT_UINT8,
	LISCR_PACKET_FORMAT_UINT16,
	LISCR_PACKET_FORMAT_UINT32,
};

typedef struct _LIScrPacket LIScrPacket;
struct _LIScrPacket
{
	char* buffer;
	LIArcReader* reader;
	LIArcWriter* writer;
};

LIScrData*
liscr_packet_new_readable (LIScrScript*       script,
                           const LIArcReader* reader);

LIScrData*
liscr_packet_new_writable (LIScrScript* script,
                           int          type);

void
liscr_packet_free (LIScrPacket* self);

#endif

/** @} */
/** @} */

